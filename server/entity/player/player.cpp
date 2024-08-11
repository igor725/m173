#include "player.h"

#include "config/config.h"
#include "containers/list/playerContainer.h"
#include "entity/manager.h"
#include "network/packets/ChatMessage.h"
#include "network/packets/Entity.h"
#include "network/packets/Handshake.h"
#include "network/packets/Object.h"
#include "network/packets/Player.h"
#include "network/packets/Window.h"
#include "network/packets/World.h"
#include "network/safesock.h"
#include "world/world.h"

#include <spdlog/spdlog.h>

class Player: public IPlayer {
  public:
  Player(SafeSocket& sock): m_selfSock(sock), m_container(&m_storage) {
    auto& dist = accessConfig().getItem("chunk.load_distance");

    m_trackDistance = dist.getValue<uint32_t>();

    m_storage.push(ItemStack(262, 64));
    m_storage.push(ItemStack(17, 64));
    m_storage.push(ItemStack(20, 64));
    m_storage.push(ItemStack(5, 64));
    m_storage.push(ItemStack(276, 1));
    m_storage.push(ItemStack(267, 1));
    m_storage.push(ItemStack(268, 1));
    m_storage.push(ItemStack(283, 1));
    m_storage.push(ItemStack(25, 4));
    m_storage.push(ItemStack(332, 16));
    m_storage.push(ItemStack(345, 1));
    m_storage.push(ItemStack(261, 1));
    m_storage.push(ItemStack(259, 1));
  }

  ~Player() {
    auto& em = accessEntityManager();

    std::unique_lock lock(m_lock);
    for (auto it = m_trackedEntities.begin(); it != m_trackedEntities.end(); ++it) {
      auto ent = em.GetEntity(*it);
      if (ent != nullptr && ent->getType() == EntityBase::Player) {
        auto ply = dynamic_cast<IPlayer*>(ent);
        ply->removeTrackedEntity(this);
      }
    }
  }

  bool sendChat(const std::wstring& message) final {
    if (message.empty()) return true; // No need to send those
    Packet::ToClient::ChatMessage wdata_cm(message);
    return wdata_cm.sendTo(getSocket());
  }

  bool doLoginProcess(const std::wstring& name) final {
    m_name = name;

    {
      Packet::ToClient::LoginRequest wdata_lr(this->getEntityId(), L"Yuck fou", this->getDimension());
      wdata_lr.sendTo(m_selfSock);
    }

    auto& spawn = accessWorld().getSpawnPoint();
    setSpawnPos(spawn);
    teleportPlayer(spawn);
    updateInventory();
    m_bLoggedIn = true;
    return true;
  }

  PlayerStorage& getStorage() final { return m_storage; }

  bool resendItem(const ItemStack& is) final {
    Packet::ToClient::SetSlotWindow wdata_ss(0, m_container.getStorageItemSlotId(is), is);
    return wdata_ss.sendTo(m_selfSock);
  }

  bool updateInventory() final {
    Packet::ToClient::ItemsWindow wdata(m_container);
    return wdata.sendTo(m_selfSock);
  }

  bool respawn() final {
    using namespace Packet::ToClient;

    m_health = m_maxHealth;
    m_storage.clear();
    updateInventory();

    // Mojang... Just why...
    EntityDestroy wdata_es(getEntityId());
    sendToTrackedPlayers(wdata_es, false);
    PlayerSpawn wdata_ps(this);
    sendToTrackedPlayers(wdata_ps, false);

    updateEquipedItem(); // Should be called after player's spawn packet

    PlayerRespawn wdata(m_dimension);
    return wdata.sendTo(m_selfSock);
  }

  bool setHealth(int16_t health) final {
    using namespace Packet::ToClient;
    auto prevHealth = m_health;

    PlayerHealth wdata_ph(m_health = std::min(health, m_maxHealth));
    if (m_health >= prevHealth) {
      return wdata_ph.sendTo(m_selfSock);
    }

    EntityStatus wdata_es(getEntityId(), m_health > 0 ? EntityStatus::Hurted : EntityStatus::Dead);

    if (m_health <= 0) { // Prevent client-side loot dropping from players
      EntityEquipment wdata_eq(getEntityId(), 0, {-1});
      sendToTrackedPlayers(wdata_eq, false);
    }

    if (wdata_ph.sendTo(m_selfSock)) {
      sendToTrackedPlayers(wdata_es, true);
      return true;
    }

    return false;
  }

  bool teleportPlayer(const DoubleVector3& pos) final {
    setPosition(pos);
    return updPlayerPos();
  }

  bool teleportPlayer(const IntVector3& pos) final {
    setPosition(DoubleVector3 {
        static_cast<double_t>(pos.x),
        static_cast<double_t>(pos.y),
        static_cast<double_t>(pos.z),
    });
    return updPlayerPos();
  }

  void setPosition(const DoubleVector3& pos) override final {
    EntityBase::setPosition(pos);
    m_stance = pos.y;
  }

  bool updateWorldChunks(bool force) final {
    std::unique_lock lock(m_lock);

    const auto prevchunk_pos = IWorld::Chunk::toChunkCoords(IntVector2 {
        static_cast<int32_t>(std::round(m_prevPosition.x)),
        static_cast<int32_t>(std::round(m_prevPosition.z)),
    });

    const auto currchunk_pos = IWorld::Chunk::toChunkCoords(IntVector2 {
        static_cast<int32_t>(std::round(m_position.x)),
        static_cast<int32_t>(std::round(m_position.z)),
    });

    if (!force && (currchunk_pos.x == prevchunk_pos.x && currchunk_pos.z == prevchunk_pos.z)) return true;

    for (auto it = m_loadedChunks.begin(); it != m_loadedChunks.end();) {
      const auto diff = IntVector2 {it->x - currchunk_pos.x, it->z - currchunk_pos.z};
      const auto dist = std::sqrt((diff.x * diff.x) + (diff.z * diff.z));
      if (dist > m_trackDistance * 1.5) {
        spdlog::trace("Unloading distant chunk Vec2({}, {}), distance: {}", it->x, it->z, dist);
        Packet::ToClient::PreChunk wdata_uc(*it, false);
        wdata_uc.sendTo(m_selfSock);
        it = m_loadedChunks.erase(it);
        continue;
      }

      ++it;
    }

    auto& world = accessWorld();

    const auto movedir = IntVector2 {
        .x = currchunk_pos.x - prevchunk_pos.x,
        .z = currchunk_pos.z - prevchunk_pos.z,
    };

    auto omod = IntVector2 {};
    auto dmod = IntVector2 {};

    if (movedir.x > 0) {
      omod = {-1, -1};
      dmod = {+1, +1};
    } else if (movedir.x < 0) {
      omod = {-1, -1};
      dmod = {+1, +1};
    } else if (movedir.z > 0) {
      omod = {-1, +1};
      dmod = {+1, +1};
    } else if (movedir.z < 0) {
      omod = {-1, -1};
      dmod = {+1, -1};
    } else {
      omod = {-1, -1};
      dmod = {1, 1};
    }

    auto dist = static_cast<int32_t>(m_trackDistance);

    const IntVector2 opos = {
        .x = currchunk_pos.x + omod.x * dist,
        .z = currchunk_pos.z + omod.z * dist,
    };

    const IntVector2 dpos = {
        .x = currchunk_pos.x + dmod.x * dist,
        .z = currchunk_pos.z + dmod.z * dist,
    };

    for (int32_t cx = opos.x; cx <= dpos.x; ++cx) {
      for (int32_t cz = opos.z; cz <= dpos.z; ++cz) {
        IntVector2 chunkpos = {cx, cz};
        if (isChunkAlreadyLoaded(chunkpos)) continue;
        spdlog::trace("Loading chunk Vec2({}:{})", cx, cz);

        m_loadedChunks.push_back(chunkpos);
        auto chunk = world.getChunk(chunkpos);

        Packet::ToClient::PreChunk wdata_pc(chunkpos, true);
        if (!wdata_pc.sendTo(m_selfSock)) return false;

        if (chunk == nullptr) {
          chunk = world.allocChunk(chunkpos);
          chunk->m_light.fill(Nible(15, 15)); // All fullbright for now
          chunk->m_sky.fill(Nible(15, 15));

          for (int32_t x = 0; x < 16; ++x) {
            for (int32_t y = 0; y < 4; ++y) {
              for (int32_t z = 0; z < 16; ++z) {
                chunk->m_blocks[chunk->getLocalIndex({x, y, z})] = y < 1 ? 7 : y < 3 ? 3 : 2;
              }
            }
          }
        }

        unsigned long gzsize;
        const auto    gzchunk = world.compressChunk(chunk, gzsize);

        Packet::ToClient::MapChunk wdata_mc({cx << 4, 0, cz << 4}, CHUNK_DIMS, gzsize);
        if (!wdata_mc.sendTo(m_selfSock)) return false;
        if (!m_selfSock.write(gzchunk, gzsize)) return false;
      }
    }

    lock.unlock();
    return updateTrackedEntities();
  }

  bool isTrackingEntity(EntityId eid) final {
    std::unique_lock lock(m_lock);

    for (auto it = m_trackedEntities.begin(); it != m_trackedEntities.end(); ++it) {
      if (*it == eid) return true;
    }

    return false;
  }

  bool addTrackedEntity(EntityBase* ent) final {
    if (!m_bLoggedIn || ent == this || !isEntityCloseEnough(ent)) return false;

    {
      std::unique_lock lock(m_lock);

      auto eid = ent->getEntityId();

      if (isTrackingEntity(eid)) return true;
      m_trackedEntities.push_back(eid);
    }

    switch (auto t = ent->getType()) {
      case EntityBase::Player: {
        auto ply = dynamic_cast<IPlayer*>(ent);
        ply->addTrackedEntity(this);

        Packet::ToClient::PlayerSpawn wdata_spawn(ply);
        wdata_spawn.sendTo(m_selfSock);
      } break;
      case EntityBase::Object: {
        Packet::ToClient::ObjectSpawn wdata_osp(dynamic_cast<ObjectBase*>(ent));
        wdata_osp.sendTo(m_selfSock);
      } break;

      default: {
        spdlog::warn("Unhandled tracked entity type {}!", (int8_t)t);
      } break;
    }

    return true;
  }

  auto removeTrackedEntity(std::vector<EntityId>::iterator it) {
    auto eid = *it;

    Packet::ToClient::EntityDestroy wdata_ed(eid);
    wdata_ed.sendTo(m_selfSock);

    auto nit = m_trackedEntities.erase(it);

    if (auto ent = accessEntityManager().GetEntity(eid)) {
      switch (auto t = ent->getType()) {
        case EntityBase::Player: {
          auto ply = dynamic_cast<IPlayer*>(ent);
          ply->removeTrackedEntity(this);
        } break;

        default: {
          spdlog::warn("Unhandled entity type: {}!", (int8_t)t);
        } break;
      }
    }

    return nit;
  }

  bool removeTrackedEntity(EntityBase* ent) final {
    if (ent == this) return false;
    std::unique_lock lock(m_lock);

    auto eid = ent->getEntityId();

    for (auto it = m_trackedEntities.begin(); it != m_trackedEntities.end(); ++it) {
      if (*it == eid) {
        it = removeTrackedEntity(it);
        return true;
      }
    }

    return false;
  }

  void sendToTrackedPlayers(PacketWriter& pw, bool self) final {
    if (self) pw.sendTo(m_selfSock);

    std::unique_lock lock(m_lock);

    auto& em = accessEntityManager();

    for (auto it = m_trackedEntities.begin(); it != m_trackedEntities.end();) {
      auto ent = em.GetEntity(*it);

      if (!ent) {
        it = removeTrackedEntity(it);
        continue;
      }

      if (ent->getType() == EntityBase::Player) {
        pw.sendTo(dynamic_cast<IPlayer*>(ent)->getSocket());
      }

      ++it;
    }
  }

  bool isEntityCloseEnough(EntityBase* ent) {
    // 1.5 modifier there just in case, won't hurt much to have a bit more tracked entities
    return ent->getPosition().distanceToNoHeight(m_position) < m_trackDistance * 1.5 * 16.0;
  }

  bool updateTrackedEntities() final {
    auto& em = accessEntityManager();

    {
      std::unique_lock lock(m_lock);

      for (auto it = m_trackedEntities.begin(); it != m_trackedEntities.end();) {
        auto ent = em.GetEntity(*it);
        if (ent == nullptr || !isEntityCloseEnough(ent)) {
          it = removeTrackedEntity(it);
          continue;
        }

        ++it;
      }
    }

    em.IterEntities([this](EntityBase* ent) -> bool {
      if (ent != this && isEntityCloseEnough(ent)) addTrackedEntity(ent);
      return true;
    });

    return true;
  }

  bool updPlayerPos() final {
    // Receiving this packet by client concludes terrain downloading state
    Packet::ToClient::PlayerPosAndLook wdata_pl(this);
    return wdata_pl.sendTo(m_selfSock);
  }

  bool addVelocity(const DoubleVector3& motion) final {
    Packet::ToClient::EntityVelocity wdata_ev(getEntityId(), motion);
    return wdata_ev.sendTo(m_selfSock);
  }

  bool setSpawnPos(const IntVector3& pos) final {
    Packet::ToClient::SpawnPosition wdata_sp(pos);
    return wdata_sp.sendTo(m_selfSock);
  }

  bool setTime(int16_t time) final {
    Packet::ToClient::TimeUpdate wdata(time);
    return wdata.sendTo(m_selfSock);
  }

  bool canHitEntity() final {
    auto time = std::chrono::system_clock::now();
    auto nhit = std::chrono::system_clock::from_time_t(m_nextHit);

    if (nhit < time) {
      m_nextHit = std::chrono::system_clock::to_time_t(time + std::chrono::milliseconds(1200));
      return true;
    }

    return false;
  }

  void updateEquipedItem() final {
    Packet::ToClient::EntityEquipment wdata_eq(getEntityId(), 0, getHeldItem());
    sendToTrackedPlayers(wdata_eq, false);
  }

  bool resendHeldItem() final {
    Packet::ToClient::SetSlotWindow wdata_ss(0, 36 + m_heldSlot, getHeldItem());
    return wdata_ss.sendTo(m_selfSock);
  }

  SlotId getHeldSlot() const final { return m_heldSlot; }

  ItemStack& getHeldItem() final { return m_storage.getByOffset(m_storage.getHotbarOffset() + m_heldSlot); }

  bool setHeldSlot(SlotId slot) final {
    if (slot < 0 || slot > 8) return false;
    m_heldSlot = slot;

    updateEquipedItem();
    return true;
  }

  void setStance(double_t stance) final { m_stance = stance; }

  double_t getStance() const final { return m_stance; }

  SafeSocket& getSocket() const final { return m_selfSock; }

  const std::wstring& getName() const final { return m_name; }

  private:
  bool isChunkAlreadyLoaded(const IntVector2& pos) {
    std::unique_lock lock(m_lock);

    for (auto it = m_loadedChunks.begin(); it != m_loadedChunks.end(); ++it) {
      if (it->x == pos.x && it->z == pos.z) return true;
    }

    return false;
  }

  const int16_t m_maxHealth = 20;

  bool                    m_bLoggedIn = false;
  SlotId                  m_heldSlot  = 0;
  SafeSocket&             m_selfSock;
  int64_t                 m_nextHit       = 0;
  double_t                m_stance        = 0.0;
  double_t                m_trackDistance = 0.0;
  std::wstring            m_name;
  std::vector<IntVector2> m_loadedChunks;
  std::vector<EntityId>   m_trackedEntities;
  std::recursive_mutex    m_lock;
  PlayerStorage           m_storage;
  PlayerContainer         m_container;
};

std::unique_ptr<IPlayer> createPlayer(SafeSocket& sock) {
  return std::make_unique<Player>(sock);
}
