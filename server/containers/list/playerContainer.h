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

  SlotId getItemSlotByItemStack(const ItemStack& is) final;

  ItemStack& getHotbarItem(uint8_t iid) final;

  bool push(const ItemStack& is, SlotId* sid, SlotId prioritySlot = -1) final;

  SlotId getStorageItemSlotId(const ItemStack& is);

  private:
  CraftingStorage<2, 2> m_crafting;
  CraftingResultStorage m_craftRes;
  PlayerStorage*        m_storage;
};
