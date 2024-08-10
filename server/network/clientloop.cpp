#include "clientloop.h"

#include "commands/handler.h"
#include "entity/manager.h"
#include "entity/player/player.h"
#include "ids.h"
#include "packets/ChatMessage.h"
#include "packets/Entity.h"
#include "packets/Handshake.h"
#include "packets/Player.h"
#include "packets/Window.h"
#include "packets/World.h"
#include "safesock.h"
#include "world/world.h"
#include "zlibpp/zlibpp.h"

#include <chrono>
#include <format>
#include <fstream>
#include <spdlog/spdlog.h>
#include <thread>
#include <zlib.h>

class UnknownPacketException: public std::exception {
  public:
  UnknownPacketException(int8_t id) { m_what = std::format("Unknown packet id {:02x}", id); }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

class UngracefulClosingException: public std::exception {
  public:
  UngracefulClosingException() { m_what = "Connection was closed ungracefully!"; }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

class HackedClientException: public std::exception {
  public:
  enum Reasons {
    WrongBlockPlace,
    WrongHoldSlot,
    _ReasonsCount,
  };

  HackedClientException(Reasons r) { m_what = std::format("It appear your client is hacked! ({})", ReasonNames[r]); }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  const char* ReasonNames[_ReasonsCount] = {
      "WrongBlockPlace",
      "WrongHoldSlot",
  };

  std::string m_what;
};

class InvalidEntityIndexException: public std::exception {
  public:
  InvalidEntityIndexException(EntityId id) { m_what = std::format("Invalid entity id ({}) passed to EntityAction packet!", id); }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

bool isBlockInteractive(BlockId block) {
  switch (block) {
    case 23:
    case 25:
    case 26:
    case 54:
    case 58:
    case 61:
    case 62:
    case 64:
    case 71:
    case 77:
    case 84: return true;

    default: return false;
  }
}

ClientLoop::ClientLoop(sockpp::tcp_socket& sock, sockpp::inet_address& addr) {
  std::thread reader(ThreadLoop, std::move(sock), std::move(addr));
  reader.detach();
}

void ClientLoop::ThreadLoop(sockpp::tcp_socket sock, sockpp::inet_address addr) {
  SafeSocket ss(std::move(sock), std::move(addr));

  IPlayer* linkedEntity = nullptr;

  const auto joinTime = std::chrono::system_clock::now();
  const auto pingFreq = std::chrono::seconds(1);

  auto lastInPing = joinTime;
  auto nextPing   = joinTime;

  bool posUpdated = false, lookUpdated = false;

  int32_t posUpdateNum = 0;

  try {
    PacketId id;
    while (ss.read(&id, sizeof(PacketId))) {
      const auto currTime = std::chrono::system_clock::now();

      spdlog::trace("Received packet {:02x} from {}", id, addr.to_string());

      switch (id) {
        case Packet::IDs::Login: {
          Packet::FromClient::LoginRequest data(ss);

          auto entId = accessEntityManager().AddEntity(createPlayer(ss));

          linkedEntity = dynamic_cast<IPlayer*>(accessEntityManager().GetEntity(entId));

          linkedEntity->doLoginProcess(data.getName());
          linkedEntity->setTime(accessWorld().getTime());
          linkedEntity->updateWorldChunks(true);
        } break;
        case Packet::IDs::Handshake: {
          Packet::FromClient::Handshake data(ss);

          Packet::ToClient::Handshake wdata(L"-");
          wdata.sendTo(ss);
        } break;
        case Packet::IDs::ChatMessage: {
          Packet::FromClient::ChatMessage data(ss);

          auto message = data.getMessage();

          if (message.starts_with(L'/')) {
            std::wstring out;
            if (!accessCommandHandler().execute(linkedEntity, message, out)) {
              out = L"Command execution failed!";
            }

            linkedEntity->sendChat(out);
          } else {
            Packet::ToClient::ChatMessage wdata(std::format(L"<{}>: {}", linkedEntity->getName(), message));

            accessEntityManager().IterPlayers([&wdata](IPlayer* player) -> bool {
              wdata.sendTo(player->getSocket());
              return true;
            });
          }
        } break;
        case Packet::IDs::EntityUse: {
          Packet::FromClient::EntityClick data(ss);

          auto target = accessEntityManager().GetEntity(data.getTarget());
          if (target == nullptr) throw InvalidEntityIndexException(data.getTarget());

          if (data.isLeftClick() && linkedEntity->canHitEntity()) {
            switch (target->getType()) {
              case EntityBase::Player: {
                auto ply_target = dynamic_cast<IPlayer*>(target);
                ply_target->setHealth(ply_target->getHealth() - 2 /* todo different damage depends on held item */);

                auto& kicker_pos   = linkedEntity->getPosition();
                auto& receiver_pos = ply_target->getPosition();

                constexpr double_t knock_strength = 0.25; // todo different values? may be random?

                const DoubleVector3 knockback = {
                    .x = (receiver_pos.x - kicker_pos.x) * knock_strength,
                    .y = knock_strength,
                    .z = (receiver_pos.z - kicker_pos.z) * knock_strength,
                };

                ply_target->addVelocity(knockback);
              } break;

              default: {
                spdlog::warn("Unknown entity clicked!");
              } break;
            }
          } else {
            // todo eh?..
          }
        } break;
        case Packet::IDs::PlayerRespawn: {
          Packet::FromClient::Respawn data(ss);
          linkedEntity->respawn();
          linkedEntity->teleportPlayer(accessWorld().getSpawnPoint());
        } break;
        case Packet::IDs::PlayerFall: {
          Packet::FromClient::PlayerFall data(ss);
        } break;
        case Packet::IDs::PlayerPos: {
          Packet::FromClient::PlayerPos data(ss);
          linkedEntity->setPosition(data.getPosition());
          linkedEntity->setStance(data.getStance());
          posUpdated = true;
        } break;
        case Packet::IDs::PlayerLook: {
          Packet::FromClient::PlayerLook data(ss);
          linkedEntity->setRotation(data.getAngle());
          lookUpdated = true;
        } break;
        case Packet::IDs::PlayerPnL: {
          Packet::FromClient::PlayerPosAndLook data(ss);
          linkedEntity->setPosition(data.getPosition());
          linkedEntity->setStance(data.getStance());
          linkedEntity->setRotation(data.getAngle());
          posUpdated = lookUpdated = true;
        } break;
        case Packet::IDs::PlayerDig: {
          Packet::FromClient::PlayerDig data(ss);

          if (data.isDiggingFinished()) { // todo dig server event
            accessWorld().setBlock(data.getPosition(), 0, 0);

            Packet::ToClient::BlockChange wdata(data.getPosition(), 0, 0);
            linkedEntity->sendToTrackedPlayers(wdata, true);
          } else if (data.isDroppingBlock()) {
            Packet::ToClient::ChatMessage wdata(L"* Item dropping is not implemented yet :(");
            wdata.sendTo(linkedEntity->getSocket());
            // todo drop block
          }
        } break;
        case Packet::IDs::BlockPlace: {
          Packet::FromClient::BlockPlace data(ss);

          if (auto activatedBlock = accessWorld().getBlock(data.getClickPosition())) {
            if (isBlockInteractive(activatedBlock)) {
              switch (activatedBlock) {
                case 23: {
                  Packet::ToClient::OpenWindow wdata(1, 3, "Dispenser", 9);
                  wdata.sendTo(ss);
                } break;
                case 58: {
                  Packet::ToClient::OpenWindow wdata(1, 1, "Crafting", 9);
                  wdata.sendTo(ss);
                } break;
                case 61: {
                  Packet::ToClient::OpenWindow wdata(1, 2, "Furnace", 9);
                  wdata.sendTo(ss);
                } break;
                case 25: { // todo actual minecraft behavior
                  Packet::ToClient::NoteBlockPlay wdata(data.getClickPosition(), Packet::ToClient::NoteBlockPlay::Harp, std::rand() % 24);
                  linkedEntity->sendToTrackedPlayers(wdata, true);
                } break;
              }
              break;
            }
          }

          if (auto id = data.getId()) {
            if (id < 1 || id > 255) break;

            auto& is = linkedEntity->getHeldItem();
            if (is.itemId != id || is.stackSize == 0) throw HackedClientException(HackedClientException::WrongBlockPlace);

            if (accessWorld().setBlock(data.getBlockPosition(), id, 0)) {
              Packet::ToClient::BlockChange wdata(data.getBlockPosition(), id, 0);
              linkedEntity->sendToTrackedPlayers(wdata, true);
              if (!is.decrementBy(1)) linkedEntity->updateEquipedItem();
            } else {
              auto bid = accessWorld().getBlock(data.getBlockPosition());

              Packet::ToClient::BlockChange wdata(data.getBlockPosition(), bid, 0);
              wdata.sendTo(linkedEntity->getSocket());
              linkedEntity->resendHeldItem();
            }

            break;
          }

          throw HackedClientException(HackedClientException::WrongBlockPlace);
        } break;
        case Packet::IDs::HoldChange: {
          Packet::FromClient::PlayerHold data(ss);
          if (!linkedEntity->setHeldSlot(data.getSlotId())) throw HackedClientException(HackedClientException::WrongHoldSlot);
        } break;
        case Packet::IDs::PlayerAnim: {
          Packet::FromClient::PlayerAnim data(ss);

          Packet::ToClient::PlayerAnim wdata(linkedEntity->getEntityId(), data.getAnimation());
          linkedEntity->sendToTrackedPlayers(wdata);
        } break;
        case Packet::IDs::PlayerAction: {
          Packet::FromClient::PlayerAction data(ss);

          switch (data.getAction()) {
            case 1: {
              linkedEntity->setCrouching(true);
            } break;
            case 2: {
              linkedEntity->setCrouching(false);
            } break;
            case 3: {
              // todo something up with the bed action
            } break;
          }
        } break;
        case Packet::IDs::CloseWindow: {
          Packet::FromClient::CloseWindow data(ss);
        } break;
        case Packet::IDs::ClickWindow: {
          Packet::FromClient::ClickWindow data(ss);
        } break;
        case Packet::IDs::ConnectionFin: {
          Packet::FromClient::Disconnect data(ss);
          ss.close();
        } break;

        default: throw UnknownPacketException(id);
      }

      if (nextPing < currTime) {
        if (linkedEntity) {
          linkedEntity->setTime(accessWorld().getTime()); // Sync world time, just in case
        }

        Packet::ToClient::KeepAlive data;
        data.sendTo(ss);

        nextPing = currTime + pingFreq;
      }

      if (linkedEntity) {
        if (posUpdated) {
          linkedEntity->updateWorldChunks();

          /**
           * @brief The original server sents teleport packet to the clients every
           * 400 movement packets, just to sync the thing. We will keep it that way too.
           */

          if (++posUpdateNum >= 400 || linkedEntity->getMoveDistance() > 4) {
            Packet::ToClient::EntitySetPos wdata_es(linkedEntity);
            linkedEntity->sendToTrackedPlayers(wdata_es);
          } else if (lookUpdated) {
            Packet::ToClient::EntityLookRM wdata_er(linkedEntity);
            linkedEntity->sendToTrackedPlayers(wdata_er);
          } else {
            Packet::ToClient::EntityRelaMove wdata_erm(linkedEntity);
            linkedEntity->sendToTrackedPlayers(wdata_erm);
          }
        } else if (lookUpdated) {
          Packet::ToClient::EntityLook wdata_el(linkedEntity);
          linkedEntity->sendToTrackedPlayers(wdata_el);
        }

        if (linkedEntity->isFlagsChanged()) {
          Packet::ToClient::EntityMeta wdata_em(linkedEntity->getEntityId());
          wdata_em.putByte(0, linkedEntity->popFlags());
          wdata_em.finish();
          linkedEntity->sendToTrackedPlayers(wdata_em);
        }
      }

      ss.pushQueue();
    }

    if (!ss.isClosed()) {
      ss.close();
      throw UngracefulClosingException();
    }
  } catch (std::exception& ex) {
    std::string_view exwhat(ex.what());

    std::wstring reason(exwhat.begin(), exwhat.end());

    auto fmtreason = std::format(L"Client thread exception: {}", reason);

    Packet::ToClient::PlayerKick wdata(fmtreason);
    wdata.sendTo(ss);

    spdlog::error("Exception thrown on {} handling: {}", addr.to_string(), ex.what());
  }

  spdlog::info("Client {} closed!", addr.to_string());
  if (linkedEntity) {
    accessEntityManager().RemoveEntity(linkedEntity->getEntityId());
  }
}
