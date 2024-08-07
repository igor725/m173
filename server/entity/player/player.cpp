#include "player.h"

#include "../../network/packets/ChatMessage.h"
#include "../../network/packets/Handshake.h"
#include "../../network/packets/Player.h"
#include "../../network/packets/SpawnPosition.h"
#include "../../network/packets/TimeUpdate.h"
#include "../../network/safesock.h"
#include "world/world.h"

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
  int16_t      m_heldItem = 0;
  SafeSocket&  m_selfSock;
  double_t     m_stance = 0.0;
  std::wstring m_name;
};

std::unique_ptr<IPlayer> createPlayer(SafeSocket& sock) {
  return std::make_unique<Player>(sock);
}
