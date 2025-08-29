#include "bmanager.h"

#include "entity/manager.h"
#include "packets/ChatMessage.h"

#include <format>

namespace BroadcastManager {
void chatToClients(const std::wstring& msg) {
  Packet::ToClient::ChatMessage wdata_cm(std::format(L"\u00a7e{}", msg));
  Entities::Access::manager().IterPlayers([&wdata_cm](Entities::PlayerBase* ply) -> bool {
    if (ply->isLoggedIn()) wdata_cm.sendTo(ply->getSocket());
    return true;
  });
}
}; // namespace BroadcastManager
