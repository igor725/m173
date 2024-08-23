#include "pig.h"

class Pig: public IPig {
  public:
  Pig(const DoubleVector3& pos): IPig() { m_position = pos, m_prevPosition = pos; }

  ~Pig() = default;

  void tick(double_t delta) {}
};

std::unique_ptr<IPig> createPig(const DoubleVector3& pos) {
  return std::make_unique<Pig>(pos);
}
