#pragma once

#include "../container.h"
#include "../storages/craftingResultStorage.h"
#include "../storages/playerStorage.h"
#include "../storages/smeltingStorage.h"

class FurnaceContainer: public IContainer {
  public:
  FurnaceContainer(PlayerStorage* pstor, SmeltingStorage* sstor);
  ~FurnaceContainer();

  uint8_t getRecipeWidth() final { return 0; }

  uint8_t getRecipeHeight() final { return 0; }

  SlotId getItemSlotById(ItemId iid) final;

  ISlot* getItemSlotByItemStack(const ItemStack& is) final;

  bool push(const ItemStack& is, SlotId* sid, SlotId prioritySlot = -1) final;

  private:
  SmeltingStorage* m_smelting;
};
