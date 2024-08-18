#pragma once

#include "../container.h"
#include "../storages/craftingResultStorage.h"
#include "../storages/craftingStorage.h"
#include "../storages/playerStorage.h"

class WorkbenchContainer: public IContainer {
  public:
  WorkbenchContainer(PlayerStorage* pstor);
  ~WorkbenchContainer();

  uint8_t getRecipeWidth() final { return 3; }

  uint8_t getRecipeHeight() final { return 3; }

  SlotId getItemSlotById(ItemId iid) final;

  SlotId getItemSlotByItemStack(const ItemStack& is) final;

  bool push(const ItemStack& is, SlotId* sid, SlotId prioritySlot = -1) final;

  SlotId getStorageItemSlotId(const ItemStack& is);

  private:
  CraftingStorage<3, 3> m_crafting;
  CraftingResultStorage m_craftRes;
};
