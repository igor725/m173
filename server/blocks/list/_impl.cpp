#include "entity/creatures/player.h"
#include "network/packets/World.h"
#include "noteBlock.h"
#include "playerui/windows/list/workbench.h"
#include "workbenchBlock.h"

#pragma region("noteBlock.h")

bool NoteBlock::blockActivated(const IntVector3& pos, EntityBase* activator) {
  Packet::ToClient::NoteBlockPlay wdata(pos, Packet::ToClient::NoteBlockPlay::Harp, std::rand() % 24);
  dynamic_cast<PlayerBase*>(activator)->sendToTrackedPlayers(wdata, true);
  return true;
}

#pragma endregion

#pragma region("workbenchBlock.h")

bool WorkbenchBlock::blockActivated(const IntVector3& pos, EntityBase* activator) {
  auto ply = dynamic_cast<PlayerBase*>(activator);
  ply->createWindow(std::make_unique<WorkbenchWindow>(&ply->getStorage()));
  return true;
}

#pragma endregion
