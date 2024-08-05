#include "player.h"

#include "../../network/packets/LoginRequest.h"
#include "../../network/packets/PlayerPosAndLook.h"
#include "../../network/packets/SpawnPosition.h"
#include "../../network/packets/TimeUpdate.h"
#include "../../network/safesock.h"

class Player: public IPlayer {
  public:
  Player(SafeSocket& sock): m_selfSock(sock) {}

  bool sendData(const void* data, size_t dsize) final { return m_selfSock.write(data, dsize); }

  bool doLoginProcess() {
    {
      std::wstring                   _str = L"Fuck you";
      Packet::ToClient::LoginRequest wdata_lr(this->getEntityId(), _str, this->getDimension());
      wdata_lr.sendTo(m_selfSock);
    }

    setSpawnPosition({0, 0, 0});
    setTime(6000);
    setPlayerPosition(this);

    return true;
  }

  bool setPlayerPosition(IPlayer* player) final {
    // Receiving this packet by client concludes terrain downloading state
    Packet::ToClient::PlayerPosAndLook wdata_pl(player);
    return wdata_pl.sendTo(m_selfSock);
  }

  bool setSpawnPosition(const IntVector3& pos) final {
    Packet::ToClient::SpawnPosition wdata_sp(pos);
    return wdata_sp.sendTo(m_selfSock);
  }

  bool setTime(int16_t time) final {
    Packet::ToClient::TimeUpdate wdata(time);
    return wdata.sendTo(m_selfSock);
  }

  private:
  SafeSocket& m_selfSock;
};

std::unique_ptr<IPlayer> createPlayer(SafeSocket& sock) {
  return std::make_unique<Player>(sock);
}
