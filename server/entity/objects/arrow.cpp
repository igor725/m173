#include "arrow.h"

namespace Entities {
class Arrow: public ArrowBase {
  public:
  Arrow(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion): ArrowBase(owner, motion) { m_position = pos, m_prevPosition = pos; }

  ~Arrow() = default;

  void tick(double_t delta) { m_shouldBeDestroyed = (m_lifeTime += delta) > 6.0; }

  private:
  double_t m_lifeTime = 0;
};

namespace Create {
std::unique_ptr<ArrowBase> arrow(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion) {
  return std::make_unique<Arrow>(pos, owner, motion);
}
} // namespace Create
} // namespace Entities
