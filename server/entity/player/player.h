#pragma once

#include "../../network/safesock.h"
#include "../entitybase.h"

#include <memory>
#include <sockpp/tcp_socket.h>

class IPlayer: public EntityBase {
  public:
  IPlayer(): EntityBase(EntityBase::Type::Player) {}

  virtual ~IPlayer() = default;

  virtual bool sendData(const void* data, size_t dsize) = 0;

  /* Environment control */
  virtual bool setTime(int16_t time) = 0;

  /* Movement control */
  virtual bool setSpawnPosition(const IntVector3& pos) = 0;

  virtual bool setPlayerPosition(IPlayer* player) = 0;

  /* Authentication */
  virtual bool doLoginProcess() = 0;
};

std::unique_ptr<IPlayer> createPlayer(SafeSocket& sock);
