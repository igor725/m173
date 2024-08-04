#pragma once

#include "../entitybase.h"

#include <memory>
#include <sockpp/tcp_socket.h>

class IPlayer: public EntityBase {
  public:
  IPlayer(): EntityBase(EntityBase::Type::Player) {}

  virtual ~IPlayer() = default;
};

std::unique_ptr<IPlayer> createPlayer(sockpp::tcp_socket& sock);
