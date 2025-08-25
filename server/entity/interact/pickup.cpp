#include "pickup.h"

#include "items/itemstack.h"

class Pickup: public IPickup {
  public:
  Pickup(const DoubleVector3& pos, const ItemStack& is): IPickup(), m_itemStack(is) { m_position = pos, m_prevPosition = pos; }

  ~Pickup() = default;

  bool isPlayer() const { return false; }

  void tick(double_t delta) { m_shouldBeDestroyed = (m_lifeTime += delta) > 5.0 * 60.0; }

  const ItemStack& getItemStack() const final { return m_itemStack; }

  private:
  double_t        m_lifeTime = 0;
  const ItemStack m_itemStack;
};

std::unique_ptr<IPickup> createPickup(const DoubleVector3& pos, const ItemStack& is) {
  return std::make_unique<Pickup>(pos, is);
}
