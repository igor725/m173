#include "player.h"

#include "network/packets/ChatMessage.h"
#include "network/packets/Handshake.h"
#include "network/packets/Player.h"
#include "network/packets/World.h"
#include "network/safesock.h"
#include "world/world.h"

#include <spdlog/spdlog.h>

class Player: public IPlayer {
  public:
  Player(SafeSocket& sock): m_selfSock(sock) {}

  bool sendData(const void* data, size_t dsize) final { return m_selfSock.write(data, dsize); }

  bool sendChat(std::wstring& message) final {
    if (message.empty()) return true; // No need to send those
    Packet::ToClient::ChatMessage wdata_cm(message);
    return wdata_cm.sendTo(getSocket());
  }

  bool doLoginProcess(const std::wstring& name) final {
    m_name = name;

    {
      Packet::ToClient::LoginRequest wdata_lr(this->getEntityId(), L"Yuck fou", this->getDimension());
      wdata_lr.sendTo(m_selfSock);
    }

    auto& spawn = accessWorld().getSpawnPoint();
    setSpawnPos(spawn);
    teleportPlayer(spawn);

    return true;
  }

  bool respawn() final {
    Packet::ToClient::PlayerRespawn wdata(m_dimension);
    return wdata.sendTo(m_selfSock);
  }

  bool setHealth(int16_t health) final {
    Packet::ToClient::PlayerHealth wdata_ph(m_health = health);
    return wdata_ph.sendTo(m_selfSock);
  }

  bool teleportPlayer(const DoubleVector3& pos) final {
    setPosition(pos);
    return updPlayerPos();
  }

  bool teleportPlayer(const IntVector3& pos) final {
    setPosition(DoubleVector3 {
        .x = static_cast<double_t>(pos.x),
        .y = static_cast<double_t>(pos.y),
        .z = static_cast<double_t>(pos.z),
    });
    return updPlayerPos();
  }

  void setPosition(const DoubleVector3& pos) override final {
    EntityBase::setPosition(pos);
    m_stance = pos.y;
  }

  bool updateWorldChunks(bool force) final {
    const auto prevchunk_pos = IWorld::Chunk::toChunkCoords({
        static_cast<int32_t>(std::round(m_prevPosition.x)),
        static_cast<int32_t>(std::round(m_prevPosition.z)),
    });

    const auto currchunk_pos = IWorld::Chunk::toChunkCoords({
        static_cast<int32_t>(std::round(m_position.x)),
        static_cast<int32_t>(std::round(m_position.z)),
    });

    if (!force && (currchunk_pos.x == prevchunk_pos.x && currchunk_pos.z == prevchunk_pos.z)) return true;

    for (auto it = m_loadedChunks.begin(); it != m_loadedChunks.end();) {
      const auto diff = IntVector2 {it->x - currchunk_pos.x, it->z - currchunk_pos.z};
      const auto dist = std::sqrt((diff.x * diff.x) + (diff.z * diff.z));
      if (dist > 10.0 * 2.0) {
        spdlog::trace("Unloading distant chunk Vec2({}, {}), distance: {}", it->x, it->z, dist);
        Packet::ToClient::PreChunk wdata_uc(*it, false);
        wdata_uc.sendTo(m_selfSock);
        it = m_loadedChunks.erase(it);
        continue;
      }

      ++it;
    }

    auto& world = accessWorld();

    const auto movedir = IntVector2 {
        .x = currchunk_pos.x - prevchunk_pos.x,
        .z = currchunk_pos.z - prevchunk_pos.z,
    };

    auto omod = IntVector2 {};
    auto dmod = IntVector2 {};

    if (movedir.x > 0) {
      omod = {-1, -1};
      dmod = {+1, +1};
    } else if (movedir.x < 0) {
      omod = {-1, -1};
      dmod = {+1, +1};
    } else if (movedir.z > 0) {
      omod = {-1, +1};
      dmod = {+1, +1};
    } else if (movedir.z < 0) {
      omod = {-1, -1};
      dmod = {+1, -1};
    } else {
      omod = {-1, -1};
      dmod = {1, 1};
    }

    const IntVector2 opos = {
        .x = currchunk_pos.x + omod.x * 10 /* todo load distance config */,
        .z = currchunk_pos.z + omod.z * 10,
    };

    const IntVector2 dpos = {
        .x = currchunk_pos.x + dmod.x * 10 /* todo load distance config */,
        .z = currchunk_pos.z + dmod.z * 10,
    };

    for (int32_t cx = opos.x; cx <= dpos.x; ++cx) {
      for (int32_t cz = opos.z; cz <= dpos.z; ++cz) {
        IntVector2 chunkpos = {cx, cz};
        if (isChunkAlreadyLoaded(chunkpos)) continue;
        spdlog::trace("Loading chunk Vec2({}:{})", cx, cz);

        m_loadedChunks.push_back(chunkpos);
        auto chunk = world.getChunk(chunkpos);

        Packet::ToClient::PreChunk wdata_pc(chunkpos, true);
        if (!wdata_pc.sendTo(m_selfSock)) return false;

        if (chunk == nullptr) {
          chunk = world.allocChunk(chunkpos);
          chunk->m_light.fill(IWorld::Chunk::BlockPack(15, 15)); // All fullbright for now

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

        Packet::ToClient::MapChunk wdata_mc({cx * 16, 0, cz * 16}, CHUNK_DIMS, gzsize);
        if (!wdata_mc.sendTo(m_selfSock)) return false;
        if (!m_selfSock.write(gzchunk, gzsize)) return false;
      }
    }

    return true;
  }

  bool updPlayerPos() final {
    // Receiving this packet by client concludes terrain downloading state
    Packet::ToClient::PlayerPosAndLook wdata_pl(this);
    return wdata_pl.sendTo(m_selfSock);
  }

  bool setSpawnPos(const IntVector3& pos) final {
    Packet::ToClient::SpawnPosition wdata_sp(pos);
    return wdata_sp.sendTo(m_selfSock);
  }

  bool setTime(int16_t time) final {
    Packet::ToClient::TimeUpdate wdata(time);
    return wdata.sendTo(m_selfSock);
  }

  int16_t getHeldItem() const final { return m_heldItem; }

  void setStance(double_t stance) final { m_stance = stance; }

  double_t getStance() const final { return m_stance; }

  SafeSocket& getSocket() const final { return m_selfSock; }

  const std::wstring& getName() const final { return m_name; }

  private:
  bool isChunkAlreadyLoaded(const IntVector2& pos) {
    for (auto it = m_loadedChunks.begin(); it != m_loadedChunks.end(); ++it) {
      if (it->x == pos.x && it->z == pos.z) return true;
    }

    return false;
  }

  int16_t                 m_heldItem = 0;
  SafeSocket&             m_selfSock;
  double_t                m_stance = 0.0;
  std::wstring            m_name;
  std::vector<IntVector2> m_loadedChunks;
};

std::unique_ptr<IPlayer> createPlayer(SafeSocket& sock) {
  return std::make_unique<Player>(sock);
}
