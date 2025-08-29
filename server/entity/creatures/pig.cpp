#include "pig.h"

namespace Entities {

class Pig: public IPig {
  public:
  Pig(const DoubleVector3& pos): IPig() { m_position = pos, m_prevPosition = pos; }

  ~Pig() = default;

  void tick(double_t delta) {}
};

namespace Create {
std::unique_ptr<IPig> pig(const DoubleVector3& pos) {
  return std::make_unique<Pig>(pos);
}
} // namespace Create

} // namespace Entities
