#pragma once

#include "../../network/safesock.h"
#include "../entitybase.h"

#include <memory>
#include <sockpp/tcp_socket.h>
#include <string>

class IPlayer: public EntityBase {
  public:
  IPlayer(): EntityBase(EntityBase::Type::Player) {}

  virtual ~IPlayer() = default;

  virtual bool sendData(const void* data, size_t dsize) = 0;

  /* Environment control */
  virtual bool setTime(int16_t time) = 0;

  /* Generic getters */

  virtual int16_t getHeldItem() const = 0;

  /* Movement control */
  virtual bool setSpawnPos(const IntVector3& pos) = 0;

  virtual void setAngle(const FloatAngle& ang) = 0;

  virtual void setPosition(const DoubleVector3& pos) = 0;

  virtual void setStance(double_t stance) = 0;

  virtual double_t getStance() const = 0;

  virtual bool updPlayerPos() = 0;

  virtual SafeSocket& getSocket() const = 0;

  /* Authentication */
  virtual const std::wstring& getName() const                          = 0;
  virtual bool                doLoginProcess(const std::wstring& name) = 0;
};

std::unique_ptr<IPlayer> createPlayer(SafeSocket& sock);
