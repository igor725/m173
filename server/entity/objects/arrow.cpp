#include "arrow.h"

#include "network/packets/Entity.h"

class Arrow: public IArrow {
  public:
  Arrow(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion): IArrow(owner, motion) { m_position = pos, m_prevPosition = pos; }

  ~Arrow() = default;

  void tick(double_t delta) { m_shouldBeDestroyed = (m_lifeTime += delta) > 6.0; }

  private:
  double_t m_lifeTime = 0;
};

std::unique_ptr<IArrow> createArrow(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion) {
  return std::make_unique<Arrow>(pos, owner, motion);
}
