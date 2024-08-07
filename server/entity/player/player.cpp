#include "player.h"

#include "../../network/packets/ChatMessage.h"
#include "../../network/packets/LoginRequest.h"
#include "../../network/packets/Player.h"
#include "../../network/packets/SpawnPosition.h"
#include "../../network/packets/TimeUpdate.h"
#include "../../network/safesock.h"

class Player: public IPlayer {
  public:
  Player(SafeSocket& sock): m_selfSock(sock) {}

  bool sendData(const void* data, size_t dsize) final { return m_selfSock.write(data, dsize); }

  bool sendChat(std::wstring& message) final {
    if (message.empty()) return true; // No need to send those
    Packet::ToClient::ChatMessage wdata_cm(message);
    return wdata_cm.sendTo(getSocket());
  }

  bool doLoginProcess(const std::wstring& name) {
    m_name = name;

    {
      std::wstring                   _str = L"Fuck you";
      Packet::ToClient::LoginRequest wdata_lr(this->getEntityId(), _str, this->getDimension());
      wdata_lr.sendTo(m_selfSock);
    }

    setSpawnPos({5, 10, 5});
    setPosition({5.0, 10.0, 5.0});
    updPlayerPos();

    return true;
  }

  void setAngle(const FloatAngle& ang) final { m_rotation = ang; }

  void setPosition(const DoubleVector3& pos) final {
    m_position = pos;
    m_stance   = pos.y;
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
  int16_t      m_heldItem;
  SafeSocket&  m_selfSock;
  double_t     m_stance;
  std::wstring m_name;
};

std::unique_ptr<IPlayer> createPlayer(SafeSocket& sock) {
  return std::make_unique<Player>(sock);
}
