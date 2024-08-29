#include "bmanager.h"

#include "entity/manager.h"
#include "packets/ChatMessage.h"

#include <format>

namespace BroadcastManager {
void chatToClients(const std::wstring& msg) {
  Packet::ToClient::ChatMessage wdata_cm(std::format(L"\u00a7e{}", msg));
  accessEntityManager().IterPlayers([&wdata_cm](PlayerBase* ply) -> bool {
    wdata_cm.sendTo(ply->getSocket());
    return true;
  });
}
}; // namespace BroadcastManager
