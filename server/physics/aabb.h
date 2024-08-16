#pragma once

#include "helper.h"

#include <cmath>

class AABB {
  public:
  AABB(): m_min(), m_max() {}

  AABB(const DoubleVector3& min, const DoubleVector3& max): m_min(min), m_max(max) {}

  bool intersectsWith(const AABB& bb) {
    return bb.m_max.x > m_min.x && bb.m_min.x < m_max.x && bb.m_max.y > m_min.y && bb.m_min.y < m_max.y && bb.m_max.z > m_min.z && bb.m_min.z < m_max.z;
  }

  bool isPointInside(const DoubleVector3& vec) {
    return vec.x > m_min.x && vec.x < m_max.x && vec.y > m_min.y && vec.y < m_max.y && vec.z > m_min.z && vec.z < m_max.z;
  }

  void setOffset(const DoubleVector3& off) { m_min += off, m_max += off; }

  private:
  DoubleVector3 m_min, m_max;
};
