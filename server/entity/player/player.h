#pragma once

#include "../entitybase.h"
#include "items/itemstack.h"
#include "network/packet.h"
#include "network/safesock.h"

#include <memory>
#include <sockpp/tcp_socket.h>
#include <string>

class IPlayer: public EntityBase {
  public:
  IPlayer(): EntityBase(EntityBase::Type::Player) {}

  virtual ~IPlayer() = default;

  virtual bool sendData(const void* data, size_t dsize) = 0;

  virtual bool sendChat(std::wstring& message) = 0;

  /* Entity tracking */

  virtual bool updateTrackedEntities()                                   = 0;
  virtual void sendToTrackedPlayers(PacketWriter& pw, bool self = false) = 0;
  virtual bool addTrackedEntity(EntityBase* ent)                         = 0;
  virtual bool removeTrackedEntity(EntityBase* ent)                      = 0;

  /* Health control functions */

  virtual bool setHealth(int16_t health) = 0;
  virtual bool respawn()                 = 0;

  /* Environment control */
  virtual bool setTime(int16_t time) = 0;

  /* Ingame world/entity manipualtions */

  virtual void updateEquipedItem()                   = 0;
  virtual bool updateWorldChunks(bool force = false) = 0;
  virtual bool canHitEntity()                        = 0;

  /* Inventory manipulations */

  virtual SlotId     getHeldSlot() const      = 0;
  virtual ItemStack& getHeldItem()            = 0;
  virtual bool       setHeldSlot(SlotId slot) = 0;
  virtual bool       updateInventory()        = 0;
  virtual bool       resendHeldItem()         = 0;

  /* Movement control */
  virtual bool addVelocity(const DoubleVector3& motion) = 0;

  virtual bool setSpawnPos(const IntVector3& pos) = 0;

  virtual void setPosition(const DoubleVector3& pos) = 0;

  virtual void setStance(double_t stance) = 0;

  virtual double_t getStance() const = 0;

  virtual bool updPlayerPos() = 0;

  virtual SafeSocket& getSocket() const = 0;

  virtual bool teleportPlayer(const DoubleVector3& pos) = 0;

  virtual bool teleportPlayer(const IntVector3& pos) = 0;

  /* Authentication */
  virtual const std::wstring& getName() const                          = 0;
  virtual bool                doLoginProcess(const std::wstring& name) = 0;
};

std::unique_ptr<IPlayer> createPlayer(SafeSocket& sock);
