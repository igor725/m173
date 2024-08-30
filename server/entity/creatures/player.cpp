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

class Player: public PlayerBase {
  public:
  Player(SafeSocket& sock, const std::wstring& name): m_selfSock(sock), m_container(&m_storage), m_name(name) {
    auto& dist = accessConfig().getItem("chunk.load_distance");

    m_trackDistance = dist.getValue<uint32_t>();
    addWindow(std::make_unique<InventoryWindow>(m_container));
  }

  ~Player() {
    auto& em = accessEntityManager();

    std::unique_lock lock(m_lockEntities);
    for (auto it = m_trackedEntities.begin(); it != m_trackedEntities.end(); ++it) {
      auto ent = em.GetEntity(*it);
      if (ent != nullptr && ent->isPlayer()) {
        auto ply = dynamic_cast<PlayerBase*>(ent);
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

    static const size_t MAX_MESSAGE_LINE = 60;

    wchar_t  lastcolor = L'f';
    uint32_t startpos  = 0;

    if (message.length() > MAX_MESSAGE_LINE) {
      while (startpos < message.length()) {
        {
          auto cpos = message.find(L'\u00a7', startpos);
          if (cpos != std::wstring_view::npos && (cpos + 1) < message.length()) {
            lastcolor = message.at(cpos + 1);
          }
        }

        auto partend = std::min(startpos + MAX_MESSAGE_LINE, message.length());

        if (lastcolor == 'f') {
          auto str = std::wstring_view(message.begin() + startpos, message.begin() + partend);
          startpos += str.length();
          if (!sendChat(str)) return false;
        } else {
          if ((partend - startpos) >= (MAX_MESSAGE_LINE - 2)) partend -= 2;
          std::wstring str = {L'\u00a7', lastcolor};
          const auto   ncl = std::wstring_view(message.begin() + startpos, message.begin() + partend);
          str.append(ncl);
          startpos += ncl.length();
          if (!sendChat(str)) return false;
        }
      }

      return true;
    }

    Packet::ToClient::ChatMessage wdata_cm(message);
    return wdata_cm.sendTo(getSocket());
  }

  bool doLoginProcess() final {
    auto& world = accessWorld();

    {
      Packet::ToClient::LoginRequest wdata_lr(this->getEntityId(), L"Yuck fou", world.getSeed(), this->getDimension());
      wdata_lr.sendTo(m_selfSock);
    }

    auto& spawn = world.getSpawnPoint();
    setSpawnPos(spawn);
    updateWorldChunks(getCurrentChunk(), getCurrentChunk());
    setTime(world.getTime());
    updateInventory();
    teleportPlayer(spawn);
    updateGroundState(false);
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
    unloadDistantChunks();
    untrackAllEntities(); // Player can't hit anyone otherwise
    updateInventory();
    updateWorldChunks(getCurrentChunk(), getCurrentChunk()); // Player hangs on loading screen otherwise

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

  void setPosition(const DoubleVector3& pos) final {
    CreatureBase::setPosition(pos);
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

  void unloadDistantChunks(bool all = false) {
    for (auto it = m_loadedChunks.begin(); it != m_loadedChunks.end();) {
      if (all == false) {
        const auto dist = it->distanceTo(getCurrentChunk());
        if (dist < m_trackDistance * 1.5) {
          ++it;
          continue;
        }
      }

      auto chunk = accessWorld().getChunk(*it);
      --chunk->m_uses;

      Packet::ToClient::PreChunk wdata_uc(*it, false);
      wdata_uc.sendTo(m_selfSock);
      it = m_loadedChunks.erase(it);
    }
  }

  bool updateWorldChunks(const IntVector2& prev, const IntVector2& curr) {
    std::unique_lock lock(m_lockChunks);
    unloadDistantChunks();

    auto& world = accessWorld();

    const auto movedir = IntVector2 {
        .x = curr.x - prev.x,
        .z = curr.z - prev.z,
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
        .x = curr.x + omod.x * dist,
        .z = curr.z + omod.z * dist,
    };

    const IntVector2 dpos = {
        .x = curr.x + dmod.x * dist,
        .z = curr.z + dmod.z * dist,
    };

    for (int32_t cx = opos.x; cx <= dpos.x; ++cx) {
      for (int32_t cz = opos.z; cz <= dpos.z; ++cz) {
        IntVector2 chunkpos = {cx, cz};
        if (isHoldingChunk(chunkpos)) continue;

        m_loadedChunks.push_back(chunkpos);
        auto chunk = world.getChunk(chunkpos);
        ++chunk->m_uses;

        Packet::ToClient::PreChunk wdata_pc(chunkpos, true);
        if (!wdata_pc.sendTo(m_selfSock)) return false;

        Packet::ToClient::MapChunk wdata_mc(chunk);
        if (!wdata_mc.sendTo(m_selfSock)) return false;
      }
    }

    return updateTrackedEntities();
  }

  void onChunkChanged(const IntVector2& prev, const IntVector2& curr) final {
    std::unique_lock lock(m_lockChunks);
    updateWorldChunks(prev, curr);
  }

  void onHealthChanged(int16_t diff, bool dead) final {
    using namespace Packet::ToClient;
    PlayerHealth wdata_ph(getHealth());
    if (diff > 0) {
      wdata_ph.sendTo(m_selfSock);
      return;
    } else if (diff == 0) {
      return;
    }

    EntityStatus wdata_es(getEntityId(), dead ? EntityStatus::Dead : EntityStatus::Hurted);

    if (dead == true) { // Prevent client-side loot dropping from players
      EntityEquipment wdata_eq(getEntityId(), 0, {-1});
      sendToTrackedPlayers(wdata_eq, false);
    }

    if (wdata_ph.sendTo(m_selfSock)) {
      sendToTrackedPlayers(wdata_es, true);
    }
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
      case EntityBase::Creature: {
        if (ent->isPlayer()) {
          auto ply = dynamic_cast<PlayerBase*>(ent);
          ply->addTrackedEntity(this);

          Packet::ToClient::PlayerSpawn wdata_spawn(ply);
          wdata_spawn.sendTo(m_selfSock);
        }
      } break;
      case EntityBase::Object: {
        Packet::ToClient::ObjectSpawn wdata_osp(dynamic_cast<ObjectBase*>(ent));
        wdata_osp.sendTo(m_selfSock);
      } break;
      case EntityBase::Thunderbolt: {
        Packet::ToClient::SpawnThunderbolt wdata_stb(ent);
        wdata_stb.sendTo(m_selfSock);
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
        case EntityBase::Creature: {
          if (ent->isPlayer()) {
            auto ply = dynamic_cast<PlayerBase*>(ent);
            ply->removeTrackedEntity(this);
          }
        } break;
        case EntityBase::Object: {
          // Don't think we have to do something there actually
        } break;
        case EntityBase::Thunderbolt: {
          // Same as above I think
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

      if (ent->isPlayer()) pw.sendTo(dynamic_cast<PlayerBase*>(ent)->getSocket());

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
    auto& item = getHeldItem();

    return Item::getById(item.itemId)->onEquipedByEntity(item, this);
  }

  bool isLocal() const final { return m_selfSock.isLocal(); }

  bool isOperator() const final { return m_bIsOperator; }

  void setStance(double_t stance) final { m_stance = stance; }

  double_t getStance() const final { return m_stance; }

  SafeSocket& getSocket() const final { return m_selfSock; }

  const std::wstring& getName() const final { return m_name; }

  EntityBase* getAttachedEntity() const final { return m_attachedEntity; }

  void setOperator(bool state) final { m_bIsOperator = state; }

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
  std::atomic<bool>       m_bLoggedIn   = false;
  std::atomic<bool>       m_bIsOperator = false;
  SlotId                  m_heldSlot    = 0;
  SafeSocket&             m_selfSock;
  int64_t                 m_nextHit        = 0;
  double_t                m_stance         = 0.0;
  double_t                m_trackDistance  = 0.0;
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

std::unique_ptr<PlayerBase> createPlayer(SafeSocket& sock, const std::wstring& name) {
  return std::make_unique<Player>(sock, name);
}
