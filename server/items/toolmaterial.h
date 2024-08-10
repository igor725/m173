#pragma once

#include <cmath>
#include <cstdint>

class ToolMaterial {
  private:
  int32_t m_harvestLevel;
  int32_t m_maxUses;
  float_t m_efficiencyOnProperMaterial;
  int32_t m_damageVsEntity;

  public:
  enum Index {
    Wood,
    Stone,
    Iron,
    Gold,
    Diamond,
  };

  ToolMaterial(int32_t harvest, int32_t uses, float_t eff, int32_t dmg) {
    m_harvestLevel               = harvest;
    m_maxUses                    = uses;
    m_efficiencyOnProperMaterial = eff;
    m_damageVsEntity             = dmg;
  }

  int32_t getMaxUses() const { return m_maxUses; }

  int32_t getDamageVsEntity() const { return m_damageVsEntity; }

  int32_t getHarvestLevel() const { return m_harvestLevel; }

  float_t getEfficiencyOnProperMaterial() const { return m_efficiencyOnProperMaterial; }

  static const ToolMaterial& select(ToolMaterial::Index idx);
};
