#include "player.h"

class Player: public IPlayer {
  public:
  Player(sockpp::tcp_socket& sock): m_selfSock(sock) {}

  private:
  sockpp::tcp_socket& m_selfSock;
};

std::unique_ptr<IPlayer> createPlayer(sockpp::tcp_socket& sock) {
  return std::make_unique<Player>(sock);
}
