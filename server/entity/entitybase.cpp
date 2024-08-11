#include "entitybase.h"

void EntityBase::setRotation(const FloatAngle& rot) {
  const FloatAngle rad = {rot.yaw / 180.0f * 3.1415f, rot.pitch / 180.0f * 3.1415f};

  m_rotation = rot;
  m_forward  = {
      -std::sin(rad.yaw) * std::cos(rad.pitch),
      -std::sin(rad.pitch),
      std::cos(rad.yaw) * std::cos(rad.pitch),
  };
}

void EntityBase::setCrouching(bool value) {
  m_prevFlags = m_flags;
  if (value)
    m_flags |= Flags::IsCrouching;
  else
    m_flags &= ~Flags::IsCrouching;
}
