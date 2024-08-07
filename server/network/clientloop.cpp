#include "clientloop.h"

#include "commands/command.h"
#include "entity/manager.h"
#include "entity/player/player.h"
#include "ids.h"
#include "packets/Animation.h"
#include "packets/Block.h"
#include "packets/ChatMessage.h"
#include "packets/EntityAction.h"
#include "packets/Handshake.h"
#include "packets/HealthUpdate.h"
#include "packets/MapChunk.h"
#include "packets/Ping.h"
#include "packets/Player.h"
#include "packets/PreChunk.h"
#include "packets/Window.h"
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

ClientLoop::ClientLoop(sockpp::tcp_socket& sock, sockpp::inet_address& addr) {
  std::thread reader(ThreadLoop, std::move(sock), std::move(addr));
  reader.detach();
}

void ClientLoop::ThreadLoop(sockpp::tcp_socket sock, sockpp::inet_address addr) {
  SafeSocket ss(std::move(sock), std::move(addr));

  IPlayer* linkedEntity = nullptr;

  const auto joinTime = std::chrono::system_clock::now();
  const auto pingFreq = std::chrono::seconds(15);

  auto lastInPing = joinTime;
  auto nextPing   = joinTime;

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

          {
            Packet::ToClient::PlayerSpawn wdata(linkedEntity);
            accessEntityManager().IterPlayers([linkedEntity, &wdata](IPlayer* ply) -> bool {
              if (ply != linkedEntity) {
                wdata.sendTo(ply->getSocket());

                Packet::ToClient::PlayerSpawn owdata(ply);
                owdata.sendTo(linkedEntity->getSocket());
              }
              return true;
            });
          }

          {
            IntVector2 chunkpos = {0, 0};
            auto&      world    = accessWorld();
            auto       chunk    = world.getChunk(chunkpos);

            linkedEntity->setTime(world.getTime());

            Packet::ToClient::PreChunk wdata_pc(chunkpos, true);
            wdata_pc.sendTo(ss);

            if (chunk == nullptr) {
              chunk = world.allocChunk(chunkpos);

              for (int32_t x = 0; x < 16; ++x) {
                for (int32_t y = 0; y < 4; ++y) {
                  for (int32_t z = 0; z < 16; ++z) {
                    chunk->m_blocks[chunk->getLocalIndex({x, y, z})] = y < 1 ? 7 : y < 3 ? 3 : 2;
                  }
                }
              }
            }

            unsigned long gzsize;
            const auto    gzchunk = world.compressChunk(chunk, gzsize);

            Packet::ToClient::MapChunk wdata_mc({0, 0, 0}, CHUNK_DIMS, gzsize);
            wdata_mc.sendTo(ss);

            ss.write(gzchunk, gzsize);
          }
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
        case Packet::IDs::PlayerRespawn: {
          Packet::FromClient::Respawn data(ss);

          /* todo: Teleport player to World's spawn point */

          Packet::ToClient::PlayerRespawn wdata(data.getDimension());
          wdata.sendTo(ss);
        } break;
        case Packet::IDs::PlayerFall: {
          Packet::FromClient::PlayerFall data(ss);
        } break;
        case Packet::IDs::PlayerPos: {
          Packet::FromClient::PlayerPos data(ss);
          linkedEntity->setPosition(data.getPosition());
          linkedEntity->setStance(data.getStance());
        } break;
        case Packet::IDs::PlayerLook: {
          Packet::FromClient::PlayerLook data(ss);
          linkedEntity->setAngle(data.getAngle());
        } break;
        case Packet::IDs::PlayerPnL: { // todo
          Packet::FromClient::PlayerPosAndLook data(ss);
          linkedEntity->setPosition(data.getPosition());
          linkedEntity->setStance(data.getStance());
          linkedEntity->setAngle(data.getAngle());
        } break;
        case Packet::IDs::PlayerDig: {
          Packet::FromClient::PlayerDig data(ss);

          if (data.isDiggingFinished()) { // todo dig server event
            accessWorld().setBlock(data.getPosition(), 0, 0);

            Packet::ToClient::BlockChange wdata(data.getPosition(), 0, 0);
            accessEntityManager().IterPlayers([&wdata](IPlayer* ply) -> bool {
              wdata.sendTo(ply->getSocket());
              return true;
            });
          }
        } break;
        case Packet::IDs::BlockPlace: {
          Packet::FromClient::BlockPlace data(ss);
        } break;
        case Packet::IDs::Animation: {
          Packet::FromClient::Animation data(ss);

          Packet::ToClient::Animation wdata(linkedEntity->getEntityId(), data.getAnimation());
          accessEntityManager().IterPlayers([&wdata, linkedEntity](IPlayer* ply) -> bool {
            if (ply != linkedEntity) wdata.sendTo(ply->getSocket());
            return true;
          });
        } break;
        case Packet::IDs::EntityAct: {
          Packet::FromClient::EntityAction data(ss);
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

        Packet::ToClient::Ping data;
        data.sendTo(ss);

        nextPing = currTime + pingFreq;
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
    // todo another cleanup? like, unloading world's chunk if no more players there
  }
}
