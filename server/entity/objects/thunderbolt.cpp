#include "thunderbolt.h"

class Thunderbolt: public IThunderbolt {
  public:
  Thunderbolt(const DoubleVector3& pos): IThunderbolt() { m_position = pos, m_prevPosition = pos; }

  ~Thunderbolt() = default;

  void tick(double_t delta) { m_shouldBeDestroyed = (m_lifeTime += delta) > 3.0; }

  private:
  double_t m_lifeTime = 0;
};

std::unique_ptr<IThunderbolt> createThunderbolt(const DoubleVector3& pos) {
  return std::make_unique<Thunderbolt>(pos);
}
