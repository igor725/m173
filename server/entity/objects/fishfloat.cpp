#include "fishfloat.h"

#include "entity/manager.h"

namespace Entities {
class FishFloat: public FishFloatBase {
  public:
  FishFloat(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion): FishFloatBase(owner, motion), m_owner(owner) {
    m_position = pos, m_prevPosition = pos;
  }

  ~FishFloat() {
    if (auto owner = dynamic_cast<PlayerBase*>(Entities::Access::manager().GetEntity(m_owner))) {
      owner->setAttachedEntity(this, true); // Destroying link to the player
    }
  }

  void tick(double_t delta) {
    if (auto owner = dynamic_cast<PlayerBase*>(Entities::Access::manager().GetEntity(m_owner))) {
      if (owner->setAttachedEntity(this)) {

        return;
      }
    }

    // Fish float will be destroyed if there is no player or it failed to attach to one
    m_shouldBeDestroyed = true;
  }

  void lure() {
    m_shouldBeDestroyed = true; // todo yep
  }

  private:
  EntityId m_owner;
};

namespace Create {
std::unique_ptr<FishFloatBase> fishFloat(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion) {
  return std::make_unique<FishFloat>(pos, owner, motion);
}
} // namespace Create
} // namespace Entities
