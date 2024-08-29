#include "runmanager.h"

#include "entity/manager.h"
#include "network/packets/Player.h"

#include <atomic>

std::atomic<bool> isServerRunning = true;

namespace RunManager {
bool isRunning() {
  return isServerRunning;
}

void stop() {
  if (isServerRunning) {
    Packet::ToClient::PlayerKick wdata_kick(L"Server stopped"); // todo move it somewhere else?
    accessEntityManager().IterPlayers([&wdata_kick](PlayerBase* ply) -> bool {
      // It's better to send one time generated packet to every player
      // than generate it for each player separately
      wdata_kick.sendTo(ply->getSocket());
      return true;
    });

    isServerRunning = false;
  }
}
}; // namespace RunManager
