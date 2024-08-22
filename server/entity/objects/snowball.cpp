#include "snowball.h"

class SnowBall: public ISnowBall {
  public:
  SnowBall(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion): ISnowBall(owner, motion) { m_position = pos, m_prevPosition = pos; }

  ~SnowBall() = default;

  void tick(double_t delta) { m_shouldBeDestroyed = (m_lifeTime += delta) > 3.3; }

  private:
  double_t m_lifeTime = 0;
};

std::unique_ptr<ISnowBall> createSnowBall(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion) {
  return std::make_unique<SnowBall>(pos, owner, motion);
}
