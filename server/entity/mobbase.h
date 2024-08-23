#pragma once

#include "creaturebase.h"
#include "helper.h"

#include <cstdint>

class MobBase: public CreatureBase {
  public:
  enum Type : int8_t {
    Unknown,
    Creeper      = 50,
    Skeleton     = 51,
    Spider       = 52,
    GiantZombie  = 53,
    Zombie       = 54,
    Slime        = 55,
    Ghast        = 56,
    ZombiePigmen = 57,
    Pig          = 90,
    Sheep        = 91,
    Cow          = 92,
    Chicken      = 93,
    Squid        = 94,
    Wolf         = 95,
  };

  MobBase(Type type): CreatureBase(EntityBase::Mob), m_mobtype(type) {}

  virtual ~MobBase() = default;

  Type getMobType() const { return m_mobtype; }

  private:
  Type m_mobtype;
};
