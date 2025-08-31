#include "minecart.h"

namespace Entities {
class Minecart: public MinecartBase {
  public:
  Minecart(const DoubleVector3& pos): MinecartBase() { m_position = pos, m_prevPosition = pos; }

  ~Minecart() = default;

  void tick(double_t delta) {}

  private:
};

namespace Create {
std::unique_ptr<MinecartBase> minecart(const DoubleVector3& pos) {
  return std::make_unique<Minecart>(pos);
}
} // namespace Create
} // namespace Entities
