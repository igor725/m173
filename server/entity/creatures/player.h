#pragma once

#include "../creaturebase.h"
#include "containers/list/playerContainer.h"
#include "containers/storages/playerStorage.h"
#include "items/itemstack.h"
#include "network/packet.h"
#include "network/safesock.h"
#include "uiwindow/uiwindow.h"

#include <memory>
#include <string>

class PlayerBase: public CreatureBase {
  public:
  enum Equipment {
    None,
    Helmet   = 1 << 0,
    Chest    = 1 << 1,
    Pants    = 1 << 2,
    Boots    = 1 << 3,
    HeldItem = 1 << 4,
  };

  PlayerBase(): CreatureBase(CreatureBase::Type::Player) {}

  virtual ~PlayerBase() = default;

  bool isPlayer() const final { return true; }

  virtual bool sendChat(const std::wstring_view message) = 0;

  /* Entity tracking */

  virtual bool updateTrackedEntities()                                   = 0;
  virtual void sendToTrackedPlayers(PacketWriter& pw, bool self = false) = 0;
  virtual bool isTrackingEntity(EntityId eid)                            = 0;
  virtual bool addTrackedEntity(EntityBase* ent)                         = 0;
  virtual bool removeTrackedEntity(EntityBase* ent)                      = 0;

  /* Health control functions */

  virtual bool respawn() = 0;

  /* Environment control */
  virtual bool setTime(int16_t time) = 0;

  /* Ingame world/entity manipualtions */

  virtual void updateEquipedItem(Equipment flags) = 0;
  virtual bool canHitEntity()                     = 0;

  /* Inventory manipulations */

  virtual ItemStack&       getHeldItem()                                          = 0;
  virtual SlotId           getHeldItemSlotId()                                    = 0;
  virtual bool             setHeldSlot(SlotId slot)                               = 0;
  virtual bool             updateInventory()                                      = 0;
  virtual bool             resendItem(const ItemStack& is)                        = 0;
  virtual PlayerStorage&   getStorage()                                           = 0;
  virtual PlayerContainer& getInventoryContainer()                                = 0;
  virtual bool             setAttachedEntity(EntityBase* ent, bool reset = false) = 0;
  virtual EntityBase*      getAttachedEntity() const                              = 0;

  /* UI manipulations */
  virtual WinId     createWindow(std::unique_ptr<UiWindow>&& win) = 0;
  virtual UiWindow* getWindowById(WinId id)                       = 0;
  virtual bool      closeWindow(WinId id)                         = 0;

  /* Movement control */
  virtual bool setSpawnPos(const IntVector3& pos) = 0;

  virtual void setPosition(const DoubleVector3& pos) = 0;

  virtual void setStance(double_t stance) = 0;

  virtual double_t getStance() const = 0;

  virtual bool updPlayerPos() = 0;

  virtual bool teleportPlayer(const DoubleVector3& pos) = 0;

  virtual bool teleportPlayer(const IntVector3& pos) = 0;

  /* Network things / Authentication */
  virtual const std::wstring& getName() const   = 0;
  virtual bool                doLoginProcess()  = 0;
  virtual SafeSocket&         getSocket() const = 0;
  virtual bool                isLocal() const   = 0;
};

std::unique_ptr<PlayerBase> createPlayer(SafeSocket& sock, const std::wstring& name);
