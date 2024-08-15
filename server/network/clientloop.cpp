#define M173_ACTIVATE_READER_API
#include "clientloop.h"

#include "blocks/block.h"
#include "commands/handler.h"
#include "entity/manager.h"
#include "entity/player/player.h"
#include "ids.h"
#include "items/item.h"
#include "packets/ChatMessage.h"
#include "packets/Entity.h"
#include "packets/Handshake.h"
#include "packets/Player.h"
#include "packets/Window.h"
#include "packets/World.h"
#include "platform/platform.h"
#include "runmanager/runmanager.h"
#include "safesock.h"
#include "world/world.h"
#include "zlibpp/zlibpp.h"

#include <chrono>
#include <format>
#include <spdlog/spdlog.h>
#include <thread>

class UnknownPacketException: public std::exception {
  public:
  UnknownPacketException(PacketId id) { m_what = std::format("Unknown packet id {:02x}", id); }

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

class GenericKickException: public std::exception {
  public:
  GenericKickException(const std::string& reason) { m_what = std::format("Kicked: {}", reason); }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

ClientLoop::ClientLoop(sockpp::tcp_socket& sock, sockpp::inet_address& addr) {
  static uint64_t playerRef = 0;

  std::thread reader(ThreadLoop, std::move(sock), std::move(addr), ++playerRef);
  accessEntityManager().AddPlayerThread(std::move(reader), playerRef);
}

void ClientLoop::ThreadLoop(sockpp::tcp_socket sock, sockpp::inet_address addr, uint64_t ref) {
  Platform::SetCurrentThreadName(std::format("Client {} reader", addr.to_string()));

  SafeSocket ss(std::move(sock), std::move(addr));

  IPlayer* linkedEntity = nullptr;

  const auto joinTime = std::chrono::system_clock::now();
  const auto pingFreq = std::chrono::seconds(1);

  auto nextPing = joinTime;

  int32_t posUpdateNum = 0;

  try {
    PacketId id;
    while (ss.read(&id, sizeof(PacketId))) {
      const auto currTime = std::chrono::system_clock::now();

      bool posUpdated = false, lookUpdated = false;

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
              out = std::format(L"\u00a7cCommand execution failed\u00a7f: {}", out);
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

          if (data.isLeftClick()) {
            if (!linkedEntity->canHitEntity()) break;

            switch (target->getType()) {
              case EntityBase::Player: {
                auto ply_target = dynamic_cast<IPlayer*>(target);

                auto& his = linkedEntity->getHeldItem();
                auto& dmg = his.getDamageVsEntity(target);

                his.hitEntity(linkedEntity, target);

                ply_target->setHealth(ply_target->getHealth() - dmg.damage);

                auto& kicker_pos   = linkedEntity->getPosition();
                auto& receiver_pos = ply_target->getPosition();

                const DoubleVector3 knockback = {
                    (receiver_pos.x - kicker_pos.x) * dmg.kbackStrength,
                    dmg.kbackStrength,
                    (receiver_pos.z - kicker_pos.z) * dmg.kbackStrength,
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
          posUpdated = lookUpdated = true;
          posUpdateNum             = 399;
        } break;
        case Packet::IDs::PlayerFall: {
          Packet::FromClient::PlayerFall data(ss);
          linkedEntity->updateGroundState(data.isOnGround());
        } break;
        case Packet::IDs::PlayerPos: {
          Packet::FromClient::PlayerPos data(ss);
          linkedEntity->setPosition(data.getPosition());
          linkedEntity->setStance(data.getStance());
          linkedEntity->updateGroundState(data.isOnGround());
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

          auto& blockPos = data.getPosition();

          if (auto prevBlock = accessWorld().getBlock(blockPos)) {
            if (data.isDiggingFinished() || Block::getById(prevBlock)->getHardness() == 0.0f) { // todo dig server event
              if (accessWorld().setBlockWithNotify(blockPos, 0, 0, linkedEntity)) {
                linkedEntity->getHeldItem().onDestroyBlock(blockPos, prevBlock, linkedEntity);
              }
            }
          } else if (data.isDroppingBlock()) {
            Packet::ToClient::ChatMessage wdata(L"* Item dropping is not implemented yet :(");
            wdata.sendTo(linkedEntity->getSocket());
            // todo drop block
          }
        } break;
        case Packet::IDs::BlockPlace: {
          Packet::FromClient::BlockPlace data(ss);

          auto& is   = linkedEntity->getHeldItem();
          auto& cpos = data.getClickPosition();

          if (data.getDirection() == -1) { // Handle item click
            if (is.itemId > 0 && Item::getById(is.itemId)->onItemRightClick(is, linkedEntity, cpos, data.getDirection())) break;
          } else {
            if (auto aid = accessWorld().getBlock(cpos)) {
              if (Block::getById(aid)->blockActivated(cpos, linkedEntity)) break;
            }

            if (is.itemId > 0 && is.useItemOnBlock(linkedEntity, cpos, data.getDirection())) {
              break;
            }
          }

          // throw HackedClientException(HackedClientException::WrongBlockPlace);
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

          if (data.getWindow() == 0) {
            if (linkedEntity->getContainer().onWindowClosed()) linkedEntity->updateInventory();
          } else {
            throw GenericKickException("Unsupported operation");
          }
        } break;
        case Packet::IDs::ClickWindow: {
          Packet::FromClient::ClickWindow data(ss);

          if (data.getWindow() == 0) {
            if (linkedEntity->getContainer().onSlotClicked(data.getSlot(), data.isRightButton(), data.isShift())) {
              Packet::ToClient::TransactionWindow wdata_tr(data.getWindow(), data.getTransactionId(), true);
              wdata_tr.sendTo(linkedEntity->getSocket());
            } else {
              Packet::ToClient::TransactionWindow wdata_tr(data.getWindow(), data.getTransactionId(), false);
              wdata_tr.sendTo(linkedEntity->getSocket());
              linkedEntity->updateInventory();
            }
          } else {
            throw GenericKickException("Unsupported operation");
          }

        } break;
        case Packet::IDs::TransactWindow: {
          Packet::FromClient::TransactionWindow data(ss);
        } break;
        case Packet::IDs::SignUpdate: {
          Packet::FromClient::SignCreate data(ss);

          Packet::ToClient::SignUpdate wdata_su(data.getPosition(), L"Signs are not\nReady to use\nJust yet,\nBe patient");
          wdata_su.sendTo(linkedEntity->getSocket());
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
            posUpdateNum = 0;
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
          Packet::ToClient::EntityMeta wdata_em(linkedEntity);
          linkedEntity->sendToTrackedPlayers(wdata_em);
        }
      }

      ss.pushQueue();
    }

    if (ss.isClosed()) {
      ss.pushQueue();
    } else {
      if (RunManager::isRunning()) {
        ss.close();
        throw UngracefulClosingException();
      }
    }
  } catch (std::exception& ex) {
    std::string_view exwhat(ex.what());

    std::wstring reason(exwhat.begin(), exwhat.end());

    Packet::ToClient::PlayerKick wdata(std::format(L"\u00a7cClientLoop exception\u00a7f: {}", reason));
    wdata.sendTo(ss);
    ss.pushQueue();

    spdlog::error("[{}] {} thrown on client handling: {}", ss.addr(), typeid(ex).name(), ex.what());
  }

  spdlog::info("Client {} closed!", ss.addr());
  if (linkedEntity) {
    accessEntityManager().RemoveEntity(linkedEntity->getEntityId());
  }

  accessEntityManager().RemovePlayerThread(ref);
}
