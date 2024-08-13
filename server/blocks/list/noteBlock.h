#pragma once

#include "../basicBlock.h"
#include "entity/player/player.h"
#include "network/packets/World.h"

class NoteBlock: public BasicBlock {
  public:
  NoteBlock(BlockId bid): BasicBlock(bid) {}

  bool blockActivated(const IntVector3& pos, EntityBase* activator) override {
    Packet::ToClient::NoteBlockPlay wdata(pos, Packet::ToClient::NoteBlockPlay::Harp, std::rand() % 24);
    dynamic_cast<IPlayer*>(activator)->sendToTrackedPlayers(wdata, true);
    return true;
  }
};
