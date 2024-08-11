#pragma once

#include "../container.h"
#include "../storages/craftingResultStorage.h"
#include "../storages/craftingStorage.h"
#include "../storages/playerStorage.h"

class PlayerContainer: public IContainer {
  public:
  PlayerContainer(PlayerStorage* inv);
  ~PlayerContainer();

  SlotId getItemSlotById(ItemId iid) final;

  ItemStack& getHotbarItem(uint8_t iid) final;

  private:
  CraftingStorage<2, 2> m_crafting;
  CraftingResultStorage m_craftRes;
  PlayerStorage*        m_storage;
};
