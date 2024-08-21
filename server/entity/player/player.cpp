#include "player.h"

#include "config/config.h"
#include "containers/slots/armorSlot.h"
#include "entity/manager.h"
#include "network/packets/ChatMessage.h"
#include "network/packets/Entity.h"
#include "network/packets/Handshake.h"
#include "network/packets/Object.h"
#include "network/packets/Player.h"
#include "network/packets/Window.h"
#include "network/packets/World.h"
#include "network/safesock.h"
#include "uiwindow/list/inventory.h"
#include "uiwindow/uiwindow.h"
#include "world/world.h"

#include <atomic>
#include <spdlog/spdlog.h>
#include <stack>

class Player: public IPlayer {
  public:
  Player(SafeSocket& sock): m_selfSock(sock), m_container(&m_storage) {
    auto& dist = accessConfig().getItem("chunk.load_distance");

    m_trackDistance = dist.getValue<uint32_t>();
    addWindow(std::make_unique<InventoryWindow>(m_container));
  }

  ~Player() {
    auto& em = accessEntityManager();

    std::unique_lock lock(m_lockEntities);
    for (auto it = m_trackedEntities.begin(); it != m_trackedEntities.end(); ++it) {
      auto ent = em.GetEntity(*it);
      if (ent != nullptr && ent->getType() == EntityBase::Player) {
        auto ply = dynamic_cast<IPlayer*>(ent);
        ply->removeTrackedEntity(this);
      }
    }
  }

  bool sendChat(const std::wstring_view message) final {
    if (message.empty()) return true; // No need to send those

    if (message.find(L'\n') != std::wstring_view::npos) {
      size_t start = 0, end = 0;

      while ((start = message.find_first_not_of(L'\n', end)) != std::wstring::npos) {
        end = message.find(L'\n', start);
        if (end == std::wstring::npos) end = message.length();
        if (!sendChat(std::wstring_view(message.begin() + start, message.begin() + end))) return false;
      }

      return true;
    }

    static const size_t MAX_MESSAGE_LENGTH = 119;
    if (message.length() > MAX_MESSAGE_LENGTH) {
      for (int32_t i = 0; i <= message.length() / MAX_MESSAGE_LENGTH; ++i) {
        auto startpos = i * MAX_MESSAGE_LENGTH;
        if (startpos >= message.length()) return true;
        auto str = std::wstring_view(message.begin() + startpos, message.begin() + std::min(startpos + MAX_MESSAGE_LENGTH, message.length()));
        if (!sendChat(str)) return false;
      }

      return true;
    }

    Packet::ToClient::ChatMessage wdata_cm(message);
    return wdata_cm.sendTo(getSocket());
  }

  bool doLoginProcess(const std::wstring& name) final {
    auto& world = accessWorld();

    m_name = name;

    {
      Packet::ToClient::LoginRequest wdata_lr(this->getEntityId(), L"Yuck fou", world.getSeed(), this->getDimension());
      wdata_lr.sendTo(m_selfSock);
    }

    auto& spawn = world.getSpawnPoint();
    setSpawnPos(spawn);
    teleportPlayer(spawn);
    m_lastGround = m_position.y;
    setTime(world.getTime());
    updateWorldChunks(true);
    updateInventory();
    m_bLoggedIn = true;
    return true;
  }

  PlayerStorage& getStorage() final { return m_storage; }

  PlayerContainer& getInventoryContainer() final { return m_container; }

  UiWindow* addWindow(std::unique_ptr<UiWindow>&& win) {
    win->setWinid(m_windows.size());
    return m_windows.emplace(std::move(win)).get();
  }

  WinId createWindow(std::unique_ptr<UiWindow>&& win) final {
    auto win_ptr = addWindow(std::move(win));

    Packet::ToClient::OpenWindow wdata_ow(win_ptr);
    wdata_ow.sendTo(m_selfSock);
    return win_ptr->getId();
  }

  UiWindow* getWindowById(WinId id) final {
    auto& cwin = m_windows.top();

    if ((*cwin) == id)
      return cwin.get();
    else {
      spdlog::warn("An attempt to operate with non-top window was made!");
      return nullptr;
    }
  }

  bool closeWindow(WinId id) final {
    auto& cwin = m_windows.top();
    if ((*cwin) != id) return false;
    cwin->onClose();
    if (id > 0) m_windows.pop(); // We should leave inventory window at the stack bottom
    return true;
  }

  bool resendItem(const ItemStack& is) final {
    auto& cwin = m_windows.top();

    if (auto slot = cwin->container()->getItemSlotByItemStack(is)) {
      if (&is == &getHeldItem())
        updateEquipedItem(Equipment::HeldItem);
      else if (slot->getSlotType() == ISlot::Armor) {
        auto armor = dynamic_cast<ArmorSlot*>(slot);
        updateEquipedItem(Equipment(1 << armor->getArmorType()));
      }
      Packet::ToClient::SetSlotWindow wdata_ss(cwin->getId(), slot->getAbsoluteSlotId(), is);
      return wdata_ss.sendTo(m_selfSock);
    }

    return false;
  }

  bool updateInventory() final {
    Packet::ToClient::ItemsWindow wdata(m_windows.top().get());
    return wdata.sendTo(m_selfSock);
  }

  void untrackAllEntities() {
    for (auto it = m_trackedEntities.begin(); it != m_trackedEntities.end();) {
      Packet::ToClient::EntityDestroy wdata_es(*it);
      wdata_es.sendTo(m_selfSock);
      it = m_trackedEntities.erase(it);
    }
  }

  bool respawn() final {
    if (m_health > 0) return false;
    using namespace Packet::ToClient;

    setHealth(m_maxHealth);
    m_storage.clear();
    m_loadedChunks.clear();
    untrackAllEntities(); // Player can't hit anyone otherwise
    updateInventory();
    updateWorldChunks(true); // Player hangs on loading screen otherwise

    // Mojang... Just why...
    EntityDestroy wdata_es(getEntityId());
    sendToTrackedPlayers(wdata_es, false);
    PlayerSpawn wdata_ps(this);
    sendToTrackedPlayers(wdata_ps, false);

    updateEquipedItem(Equipment(0xFFFF)); // Should be called after player's spawn packet

    PlayerRespawn wdata(m_dimension);
    if (!wdata.sendTo(m_selfSock)) return false;

    return teleportPlayer(accessWorld().getSpawnPoint());
  }

  bool setHealth(int16_t health) final {
    using namespace Packet::ToClient;
    auto prevHealth = m_health;

    PlayerHealth wdata_ph(m_health = std::max(int16_t(0), std::min(health, m_maxHealth)));
    if (m_health >= prevHealth) {
      return wdata_ph.sendTo(m_selfSock);
    } else if (prevHealth == m_health) {
      return true;
    }

    EntityStatus wdata_es(getEntityId(), m_health > 0 ? EntityStatus::Hurted : EntityStatus::Dead);

    if (m_health == 0) { // Prevent client-side loot dropping from players
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

  void updateGroundState(bool ground) final {
    if (m_isOnGround != ground) {
      if ((m_isOnGround = ground) == true) {
        auto fallDist = m_lastGround - m_position.y;
        if (fallDist > 2.0) {
          setHealth(m_health - std::max(0.0, std::ceil(fallDist - 3.0)));
        }
      } else {
        m_lastGround = m_position.y;
      }
    }
  }

  void setPosition(const DoubleVector3& pos) final {
    EntityBase::setPosition(pos);
    m_stance = pos.y;
  }

  bool isHoldingChunk(const IntVector2& pos) {
    if (!m_bLoggedIn) return false;
    std::unique_lock lock(m_lockChunks);

    for (auto it = m_loadedChunks.begin(); it != m_loadedChunks.end(); ++it) {
      if (it->x == pos.x && it->z == pos.z) return true;
    }

    return false;
  }

  bool updateWorldChunks(bool force) final {
    std::unique_lock lock(m_lockChunks);

    const auto prevchunk_pos = Chunk::toChunkCoords(IntVector2 {
        static_cast<int32_t>(std::round(m_prevPosition.x)),
        static_cast<int32_t>(std::round(m_prevPosition.z)),
    });

    const auto currchunk_pos = Chunk::toChunkCoords(IntVector2 {
        static_cast<int32_t>(std::round(m_position.x)),
        static_cast<int32_t>(std::round(m_position.z)),
    });

    if (!force && (currchunk_pos.x == prevchunk_pos.x && currchunk_pos.z == prevchunk_pos.z)) return true;

    for (auto it = m_loadedChunks.begin(); it != m_loadedChunks.end();) {
      const auto diff = IntVector2 {it->x - currchunk_pos.x, it->z - currchunk_pos.z};
      const auto dist = std::sqrt((diff.x * diff.x) + (diff.z * diff.z));
      if (dist > m_trackDistance * 1.5) {
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
        if (isHoldingChunk(chunkpos)) continue;

        m_loadedChunks.push_back(chunkpos);
        auto& chunk = world.getChunk(chunkpos);

        Packet::ToClient::PreChunk wdata_pc(chunkpos, true);
        if (!wdata_pc.sendTo(m_selfSock)) return false;

        Packet::ToClient::MapChunk wdata_mc({cx << 4, 0, cz << 4}, CHUNK_DIMS, chunk);
        if (!wdata_mc.sendTo(m_selfSock)) return false;
      }
    }

    return true;
  }

  bool isTrackingEntity(EntityId eid) final {
    std::unique_lock lock(m_lockEntities);

    for (auto it = m_trackedEntities.begin(); it != m_trackedEntities.end(); ++it) {
      if (*it == eid) return true;
    }

    return false;
  }

  bool addTrackedEntity(EntityBase* ent) final {
    if (!m_bLoggedIn || ent == this || !isEntityCloseEnough(ent)) return false;

    {
      std::unique_lock lock(m_lockEntities);

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
    std::unique_lock lock(m_lockEntities);

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

    std::unique_lock lock(m_lockEntities);

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
      std::unique_lock lock(m_lockEntities);

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

  void updateEquipedItem(Equipment flags) final {
    if (flags & Equipment::HeldItem) {
      Packet::ToClient::EntityEquipment wdata_eq(getEntityId(), 0, getHeldItem());
      sendToTrackedPlayers(wdata_eq, false);
    }

    auto& stor = getStorage();

    if (flags & Equipment::Helmet) {
      Packet::ToClient::EntityEquipment wdata_eq(getEntityId(), 4, stor.getByOffset(stor.getArmorOffset() + 0));
      sendToTrackedPlayers(wdata_eq, false);
    }

    if (flags & Equipment::Chest) {
      Packet::ToClient::EntityEquipment wdata_eq(getEntityId(), 3, stor.getByOffset(stor.getArmorOffset() + 1));
      sendToTrackedPlayers(wdata_eq, false);
    }

    if (flags & Equipment::Pants) {
      Packet::ToClient::EntityEquipment wdata_eq(getEntityId(), 2, stor.getByOffset(stor.getArmorOffset() + 2));
      sendToTrackedPlayers(wdata_eq, false);
    }

    if (flags & Equipment::Boots) {
      Packet::ToClient::EntityEquipment wdata_eq(getEntityId(), 1, stor.getByOffset(stor.getArmorOffset() + 3));
      sendToTrackedPlayers(wdata_eq, false);
    }
  }

  ItemStack& getHeldItem() final { return m_container.getHotbarItem(m_heldSlot); }

  SlotId getHeldItemSlotId() final { return m_container.getItemSlotIdByItemStack(getHeldItem()); }

  bool setHeldSlot(SlotId slot) final {
    if (slot < 0 || slot > 8) return false;
    m_heldSlot = slot;

    updateEquipedItem(Equipment::HeldItem);
    return true;
  }

  bool isLocal() const final { return m_selfSock.isLocal(); }

  void setStance(double_t stance) final { m_stance = stance; }

  double_t getStance() const final { return m_stance; }

  SafeSocket& getSocket() const final { return m_selfSock; }

  const std::wstring& getName() const final { return m_name; }

  EntityBase* getAttachedEntity() const final { return m_attachedEntity; }

  bool setAttachedEntity(EntityBase* ent, bool reset) final {
    if (!reset && m_attachedEntity == nullptr) {
      // todo some callback for attached entity?
      m_attachedEntity = ent;
      return true;
    } else if (reset && m_attachedEntity == ent) {
      m_attachedEntity = nullptr;
      return true;
    }

    return m_attachedEntity == ent;
  }

  private:
  std::atomic<bool>       m_bLoggedIn = false;
  SlotId                  m_heldSlot  = 0;
  SafeSocket&             m_selfSock;
  int64_t                 m_nextHit       = 0;
  double_t                m_stance        = 0.0;
  double_t                m_trackDistance = 0.0;
  double_t                m_lastGround;
  EntityBase*             m_attachedEntity = nullptr;
  std::wstring            m_name;
  std::vector<IntVector2> m_loadedChunks;
  std::vector<EntityId>   m_trackedEntities;
  std::recursive_mutex    m_lockEntities;
  std::recursive_mutex    m_lockChunks;
  PlayerStorage           m_storage;
  PlayerContainer         m_container;

  std::stack<std::unique_ptr<UiWindow>> m_windows;
};

std::unique_ptr<IPlayer> createPlayer(SafeSocket& sock) {
  return std::make_unique<Player>(sock);
}
