#define M173_ACTIVATE_READER_API
#include "clientloop.h"

#include "blocks/block.h"
#include "bmanager.h"
#include "commands/handler.h"
#include "config/config.h"
#include "entity/creatures/player.h"
#include "entity/manager.h"
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
#include "script/event.h"
#include "script/script.h"
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
  GenericKickException(const std::string& reason) { m_what = reason; }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

static std::atomic<uint32_t> g_clientCount = 0;

ClientLoop::ClientLoop(sockpp::tcp_socket& sock, sockpp::inet_address& addr) {
  static uint64_t playerRef  = 0;
  static uint32_t maxClients = accessConfig().getItem("bind.max_clients").getValue<uint32_t>();

  if (g_clientCount >= maxClients) {
    Packet::ToClient::PlayerKick wdata_kick(L"The server is full!");
    wdata_kick.sendTo(sock);
    sock.shutdown(SHUT_WR);
    return;
  }

  ++g_clientCount;
  std::thread reader(ThreadLoop, std::move(sock), std::move(addr), ++playerRef);
  accessEntityManager().AddPlayerThread(std::move(reader), playerRef);
}

void ClientLoop::ThreadLoop(sockpp::tcp_socket sock, sockpp::inet_address addr, uint64_t ref) {
  Platform::SetCurrentThreadName(std::format("Client {} reader", addr.to_string()));

  SafeSocket ss(std::move(sock), std::move(addr));

  PlayerBase* linkedPlayer = nullptr;

  const auto joinTime = std::chrono::system_clock::now();
  const auto pingFreq = std::chrono::seconds(1);

  auto nextPing = joinTime;

  int32_t posUpdateNum = 399;

  try {
    PacketId id;
    while (ss.pushQueue() && ss.read(&id, sizeof(PacketId))) {
      const auto currTime = std::chrono::system_clock::now();

      bool posUpdated = false, lookUpdated = false;

      spdlog::trace("Received packet {:02x} from {}", id, addr.to_string());

      if (linkedPlayer == nullptr) {
        switch (id) {
          case Packet::IDs::Login:
          case Packet::IDs::Handshake: {
          } break;

          default: {
            throw GenericKickException("You should authorize first!");
          } break;
        }
      }

      switch (id) {
        case Packet::IDs::Login: {
          Packet::FromClient::LoginRequest data(ss);

          auto& uname = data.getName();

          std::string bname;
          bname.resize(uname.length());
          std::wcstombs(bname.data(), uname.c_str(), bname.size() - 1);

          linkedPlayer = dynamic_cast<PlayerBase*>(accessEntityManager().AddEntity(createPlayer(ss, uname)));

          auto arg = onPlayerConnectedEvent {false, "", linkedPlayer};
          accessScript().postEvent({ScriptEvent::onPlayerConnected, &arg});

          if (arg.cancelled) {
            throw GenericKickException(arg.reason);
          } else {
            linkedPlayer->doLoginProcess();

            static bool localOp = accessConfig().getItem("perms.local_op").getValue<bool>();
            if (localOp && linkedPlayer->isLocal()) linkedPlayer->setOperator(true);

            BroadcastManager::chatToClients(std::format(L"{} joined the game", uname));
            spdlog::info("Player {} ({}) just spawned!", bname, addr.to_string());
          }
        } break;
        case Packet::IDs::Handshake: {
          Packet::FromClient::Handshake data(ss);

          Packet::ToClient::Handshake wdata(L"-");
          wdata.sendTo(ss);
        } break;
        case Packet::IDs::ChatMessage: {
          Packet::FromClient::ChatMessage data(ss);

          auto& message = data.getMessage();

          if (message.starts_with(L'/')) {
            std::wstring out;
            if (!accessCommandHandler().execute(linkedPlayer, message, out)) {
              out = std::format(L"\u00a7cCommand execution failed\u00a7f: {}", out);
            }

            linkedPlayer->sendChat(out);
          } else {
            for (auto it = message.begin(); it != message.end();) { // todo more message sanitizing
              if (*it == L'\u00a7') {
                it = message.erase(it, it + std::min(ptrdiff_t(2), std::distance(it, message.end())));
                continue;
              }

              ++it;
            }

            std::wstring   fin = std::format(L"<{}>: {}", linkedPlayer->getName(), message);
            onMessageEvent arg = {false, linkedPlayer, message, fin};

            accessScript().postEvent({ScriptEvent::onMessage, &arg});
            if (!arg.cancelled) {
              Packet::ToClient::ChatMessage wdata(arg.finalMessage);

              accessEntityManager().IterPlayers([&wdata](PlayerBase* player) -> bool {
                wdata.sendTo(player->getSocket());
                return true;
              });
            }
          }
        } break;
        case Packet::IDs::EntityUse: {
          Packet::FromClient::EntityClick data(ss);

          auto target = accessEntityManager().GetEntity(data.getTarget());
          if (target == nullptr) throw InvalidEntityIndexException(data.getTarget());

          if (data.isLeftClick()) {
            if (!linkedPlayer->canHitEntity()) break;

            switch (target->getType()) {
              case EntityBase::Creature: {
                auto creat_target = dynamic_cast<CreatureBase*>(target);

                VsDamageInfo dmg;

                auto& his = linkedPlayer->getHeldItem();
                his.getDamageVsEntity(creat_target, dmg);

                his.hitEntity(linkedPlayer, creat_target);

                creat_target->setHealth(creat_target->getHealth() - dmg.damage);

                auto& kicker_pos   = linkedPlayer->getPosition();
                auto& receiver_pos = creat_target->getPosition();

                const DoubleVector3 knockback = {
                    (receiver_pos.x - kicker_pos.x) * dmg.kbackStrength,
                    dmg.kbackStrength,
                    (receiver_pos.z - kicker_pos.z) * dmg.kbackStrength,
                };

                creat_target->addVelocity(knockback);
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
          linkedPlayer->respawn();
          posUpdated = lookUpdated = true;
          posUpdateNum             = 399;
        } break;
        case Packet::IDs::PlayerFall: {
          Packet::FromClient::PlayerFall data(ss);
          linkedPlayer->updateGroundState(data.isOnGround());
        } break;
        case Packet::IDs::PlayerPos: {
          Packet::FromClient::PlayerPos data(ss);
          linkedPlayer->setPosition(data.getPosition());
          linkedPlayer->setStance(data.getStance());
          linkedPlayer->updateGroundState(data.isOnGround());
          posUpdated = true;
        } break;
        case Packet::IDs::PlayerLook: {
          Packet::FromClient::PlayerLook data(ss);
          linkedPlayer->setRotation(data.getAngle());
          lookUpdated = true;
        } break;
        case Packet::IDs::PlayerPnL: {
          Packet::FromClient::PlayerPosAndLook data(ss);
          linkedPlayer->setPosition(data.getPosition());
          linkedPlayer->setStance(data.getStance());
          linkedPlayer->setRotation(data.getAngle());
          posUpdated = lookUpdated = true;
        } break;
        case Packet::IDs::PlayerDig: {
          Packet::FromClient::PlayerDig data(ss);

          auto& blockPos = data.getPosition();

          if (auto prevBlock = accessWorld().getBlock(blockPos)) {
            if (data.isDiggingFinished() || Block::getById(prevBlock)->getHardness() == 0.0f) { // todo dig server event
              if (accessWorld().setBlockWithNotify(blockPos, 0, 0, linkedPlayer)) {
                linkedPlayer->getHeldItem().onDestroyBlock(blockPos, prevBlock, linkedPlayer);
              }
            }
          } else if (data.isDroppingBlock()) {
            Packet::ToClient::ChatMessage wdata(L"* Item dropping is not implemented yet :(");
            wdata.sendTo(linkedPlayer->getSocket());
            // todo drop block
          }
        } break;
        case Packet::IDs::BlockPlace: {
          Packet::FromClient::BlockPlace data(ss);

          auto& is   = linkedPlayer->getHeldItem();
          auto& cpos = data.getClickPosition();

          if (data.getDirection() == -1) { // Handle item click
            if (is.itemId > 0 && Item::getById(is.itemId)->onItemRightClick(is, linkedPlayer, cpos, data.getDirection())) break;
          } else {
            if (auto aid = accessWorld().getBlock(cpos)) {
              if (Block::getById(aid)->blockActivated(cpos, linkedPlayer)) break;
            }

            if (is.itemId > 0 && is.useItemOnBlock(linkedPlayer, cpos, data.getDirection())) {
              break;
            }
          }

          // throw HackedClientException(HackedClientException::WrongBlockPlace);
        } break;
        case Packet::IDs::HoldChange: {
          Packet::FromClient::PlayerHold data(ss);
          if (!linkedPlayer->setHeldSlot(data.getSlotId())) throw HackedClientException(HackedClientException::WrongHoldSlot);
        } break;
        case Packet::IDs::PlayerAnim: {
          Packet::FromClient::PlayerAnim data(ss);

          Packet::ToClient::PlayerAnim wdata(linkedPlayer->getEntityId(), data.getAnimation());
          linkedPlayer->sendToTrackedPlayers(wdata);
        } break;
        case Packet::IDs::PlayerAction: {
          Packet::FromClient::PlayerAction data(ss);

          switch (data.getAction()) {
            case 1: {
              linkedPlayer->setCrouching(true);
            } break;
            case 2: {
              linkedPlayer->setCrouching(false);
            } break;
            case 3: {
              // todo something up with the bed action
            } break;
          }
        } break;
        case Packet::IDs::CloseWindow: {
          Packet::FromClient::CloseWindow data(ss);

          if (linkedPlayer->closeWindow(data.getWindow())) linkedPlayer->updateInventory();
        } break;
        case Packet::IDs::ClickWindow: {
          Packet::FromClient::ClickWindow data(ss);

          std::array<ItemStack*, 3> update = {nullptr};
          bool                      succ   = false;

          if (auto window = linkedPlayer->getWindowById(data.getWindow())) {
            succ = window->onClick(data.getSlot(), data.isRightButton(), data.isShift(), update.data());
            if (succ == false) linkedPlayer->updateInventory();
          }

          Packet::ToClient::TransactionWindow wdata_tr(data.getWindow(), data.getTransactionId(), succ);
          wdata_tr.sendTo(linkedPlayer->getSocket());

          for (auto it = update.begin(); it != update.end() && (*it) != nullptr; ++it) {
            linkedPlayer->resendItem(**it);
          }
        } break;
        case Packet::IDs::TransactWindow: {
          Packet::FromClient::TransactionWindow data(ss);
        } break;
        case Packet::IDs::SignUpdate: {
          Packet::FromClient::SignCreate data(ss);

          Packet::ToClient::SignUpdate wdata_su(data.getPosition(), L"Signs are not\nReady to use\nJust yet,\nBe patient");
          wdata_su.sendTo(linkedPlayer->getSocket());
        } break;
        case Packet::IDs::ConnectionFin: {
          Packet::FromClient::Disconnect data(ss);
          ss.close();
        } break;

        default: throw UnknownPacketException(id);
      }

      if (nextPing < currTime) {
        if (linkedPlayer) {
          linkedPlayer->setTime(accessWorld().getTime()); // Sync world time, just in case
        }

        Packet::ToClient::KeepAlive data;
        data.sendTo(ss);

        nextPing = currTime + pingFreq;
      }

      if (linkedPlayer) {
        if (posUpdated) {
          /**
           * @brief The original server sents teleport packet to the clients every
           * 400 movement packets, just to sync the thing. We will keep it that way too.
           */

          if (++posUpdateNum >= 400 || linkedPlayer->getMoveDistance() > 4) {
            Packet::ToClient::EntitySetPos wdata_es(linkedPlayer);
            linkedPlayer->sendToTrackedPlayers(wdata_es);
            posUpdateNum = 0;
          } else if (lookUpdated) {
            Packet::ToClient::EntityLookRM wdata_er(linkedPlayer);
            linkedPlayer->sendToTrackedPlayers(wdata_er);
          } else {
            Packet::ToClient::EntityRelaMove wdata_erm(linkedPlayer);
            linkedPlayer->sendToTrackedPlayers(wdata_erm);
          }
        } else if (lookUpdated) {
          Packet::ToClient::EntityLook wdata_el(linkedPlayer);
          linkedPlayer->sendToTrackedPlayers(wdata_el);
        }

        if (linkedPlayer->isFlagsChanged()) {
          Packet::ToClient::EntityMeta wdata_em(linkedPlayer);
          linkedPlayer->sendToTrackedPlayers(wdata_em);
        }
      }
    }

    if (ss.isClosed()) {
      ss.pushQueue();
    } else {
      if (RunManager::isRunning()) {
        ss.close();
        throw UngracefulClosingException();
      }
    }
  } catch (GenericKickException& kex) {
    std::string_view exwhat(kex.what());
    std::wstring     reason(exwhat.begin(), exwhat.end());

    Packet::ToClient::PlayerKick wdata(std::format(L"\u00a7cKicked\u00a7f: {}", reason));
    wdata.sendTo(ss);
    ss.pushQueue();
  } catch (std::exception& ex) {
    std::string_view exwhat(ex.what());
    std::wstring     reason(exwhat.begin(), exwhat.end());

    Packet::ToClient::PlayerKick wdata(std::format(L"\u00a7cClientLoop exception\u00a7f: {}", reason));
    wdata.sendTo(ss);
    ss.pushQueue();

    spdlog::error("[{}] {} thrown on client handling: {}", ss.addr(), typeid(ex).name(), ex.what());
  }

  if (linkedPlayer) {
    BroadcastManager::chatToClients(std::format(L"{} left the game", linkedPlayer->getName()));
    accessEntityManager().RemoveEntity(linkedPlayer->getEntityId());
  }

  spdlog::info("Client {} closed!", ss.addr());

  accessEntityManager().RemovePlayerThread(ref);
  --g_clientCount;
}
