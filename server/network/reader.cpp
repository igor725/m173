#include "reader.h"

#include "entity/manager.h"
#include "entity/player/player.h"
#include "ids.h"
#include "packets/Animation.h"
#include "packets/ChatMessage.h"
#include "packets/EntityAction.h"
#include "packets/Handshake.h"
#include "packets/HealthUpdate.h"
#include "packets/Kick.h"
#include "packets/LoginRequest.h"
#include "packets/MapChunk.h"
#include "packets/Ping.h"
#include "packets/Player.h"
#include "packets/PreChunk.h"
#include "packets/Respawn.h"
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

CreateReader::CreateReader(sockpp::tcp_socket& sock, sockpp::inet_address& addr) {
  std::thread reader(ThreadLoop, std::move(sock), std::move(addr));
  reader.detach();
}

void CreateReader::ThreadLoop(sockpp::tcp_socket sock, sockpp::inet_address addr) {
  SafeSocket ss(std::move(sock), std::move(addr));

  bool     isReaderRunning = true;
  IPlayer* linkedEntity    = nullptr;

  const auto pingFreq = std::chrono::seconds(2);
  auto       nextPing = std::chrono::system_clock::now() + pingFreq;

  try {
    while (isReaderRunning) {
      PacketId id;
      if (!ss.read(&id, sizeof(PacketId))) {
        isReaderRunning = false;
        break;
      }

      spdlog::trace("Received packet {:02x} from {}", id, addr.to_string());

      switch (id) {
        case Packet::IDs::KeepAlive: {
        } break;
        case Packet::IDs::Login: {
          Packet::FromClient::LoginRequest data(ss);

          auto entId = accessEntityManager().AddEntity(createPlayer(ss));

          linkedEntity = dynamic_cast<IPlayer*>(accessEntityManager().GetEntity(entId));

          linkedEntity->doLoginProcess();

          {
            IntVector2                 cpos = {0, 0};
            Packet::ToClient::PreChunk wdata_pc(cpos, true);
            wdata_pc.sendTo(ss);
          }

          {
            auto& world = accessWorld();
            auto  chunk = world.getChunk(0, 0);

            for (int32_t x = 0; x < 16; ++x) {
              for (int32_t y = 0; y < 4; ++y) {
                for (int32_t z = 0; z < 16; ++z) {
                  chunk->m_blocks[world.getIndex({x, y, z})] = y < 1 ? 7 : y < 3 ? 3 : 2;
                }
              }
            }

            unsigned long gzsize;
            const auto    gzchunk = world.compressChunk(chunk, gzsize);

            Packet::ToClient::MapChunk wdata_mc({0, 0, 0}, CHUNK_DIMS, gzsize);
            wdata_mc.sendTo(ss);

            ss.write(gzchunk, gzsize);
          }
          // {
          //   std::ifstream file("datachunk.dat", std::ios::in | std::ios::binary);

          //   file.seekg(0, std::ios::end);
          //   auto sz = file.tellg();
          //   file.seekg(0, std::ios::beg);

          //   auto array_in  = new char[sz];
          //   auto array_out = new char[sz];
          //   file.read(array_in, sz);

          //   auto compr = createCompressor();
          //   compr->setInput(array_in, sz);
          //   compr->setOutput(array_out, sz);
          //   while (!compr->tick()) {
          //     spdlog::trace("We need to compress {} more bytes", compr->getAvailableInput());
          //   }

          //   IntVector3                 cpos  = {0, 0, 0};
          //   ByteVector3                csize = {15, 1, 15};
          //   Packet::ToClient::MapChunk wdata_mc(cpos, csize, compr->getTotalOutput());
          //   wdata_mc.sendTo(ss);
          //   ss.write(array_out, compr->getTotalOutput()); // Sending the actual data
          // }

          // {
          //   Packet::ToClient::HealthUpdate wdata_hu(0);
          //   wdata_hu.sendTo(sock);
          // }
        } break;
        case Packet::IDs::Handshake: {
          Packet::FromClient::Handshake data(ss);

          std::wstring chash = L"-";

          Packet::ToClient::Handshake wdata(chash);
          wdata.sendTo(ss);
        } break;
        case Packet::IDs::ChatMessage: {
          Packet::FromClient::ChatMessage data(ss);
        } break;
        case Packet::IDs::Respawn: {
          Packet::FromClient::Respawn data(ss);

          /* todo: Teleport player to World's spawn point */

          Packet::ToClient::Respawn wdata(data.getDimension());
          wdata.sendTo(ss);
        } break;
        case Packet::IDs::PlayerFall: {
          Packet::FromClient::PlayerFall data(ss);
        } break;
        case Packet::IDs::PlayerPos: {
          Packet::FromClient::PlayerPos data(ss);
        } break;
        case Packet::IDs::PlayerLook: {
          Packet::FromClient::PlayerLook data(ss);
        } break;
        case Packet::IDs::PlayerPnL: {
          Packet::FromClient::PlayerPosAndLook data(ss);
        } break;
        case Packet::IDs::PlayerDig: {
          Packet::FromClient::PlayerDig data(ss);
        } break;
        case Packet::IDs::BlockPlace: {
          Packet::FromClient::BlockPlace data(ss);
        } break;
        case Packet::IDs::Animation: {
          Packet::FromClient::Animation data(ss);
        } break;
        case Packet::IDs::EntityAct: {
          Packet::FromClient::EntityAction data(ss);
          spdlog::info("Player performed action {}", data.getAction());
        } break;
        case Packet::IDs::CloseWindow: {
          Packet::FromClient::CloseWindow data(ss);
        } break;
        case Packet::IDs::ClickWindow: {
          Packet::FromClient::ClickWindow data(ss);
        } break;
        case Packet::IDs::Disconnect: {
          Packet::FromClient::Disconnect data(ss);
          isReaderRunning = false;
        } break;

        default: throw UnknownPacketException(id);
      }

      const auto ctime = std::chrono::system_clock::now();
      if (nextPing < ctime) {
        Packet::ToClient::Ping data;
        data.sendTo(ss);
        nextPing = ctime + pingFreq;
      }
    }
  } catch (std::exception& ex) {
    std::string_view exwhat(ex.what());

    std::wstring reason(exwhat.begin(), exwhat.end());

    auto fmtreason = std::format(L"Client thread exception: {}", reason);

    Packet::ToClient::Kick wdata(fmtreason);
    wdata.sendTo(ss);

    spdlog::error("Exception thrown on {} handling: {}", addr.to_string(), ex.what());
  }

  spdlog::info("Client {} closed!", addr.to_string());
}
