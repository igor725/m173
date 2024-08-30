#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>

#define CLASS_NO_COPY(name)                                                                                                                                    \
  name(const name&)            = delete;                                                                                                                       \
  name& operator=(const name&) = delete;

#define CLASS_NO_MOVE(name)                                                                                                                                    \
  name(name&&) noexcept            = delete;                                                                                                                   \
  name& operator=(name&&) noexcept = delete

#define _WIDELITERAL(_s) L##_s
#define WIDELITERAL(_s)  _WIDELITERAL(_s)

typedef uint32_t EntityId;
typedef uint8_t  BlockId;
typedef int16_t  ItemId;
typedef int8_t   AnimId;
typedef int8_t   WinId;
typedef uint8_t  PacketId;
typedef int16_t  SlotId;

struct IntVector2 {
  int32_t x, z;

  bool operator==(const IntVector2& other) const { return x == other.x && z == other.z; }

  float_t distanceTo(const IntVector2& other) const { return ::sqrtf((other.x - x) * (other.x - x) + (other.z - z) * (other.z - z)); }

  struct HashFunction {
    union UPack {
      size_t hash;

      struct {
        int64_t x : 32;
        int64_t z : 32;
      };
    };

    size_t operator()(const IntVector2& pos) const {
      UPack p;

      p.x = pos.x;
      p.z = pos.z;

      return p.hash;
    }
  };
};

struct ByteVector3 {
  int8_t x, y, z;
};

struct IntVector3 {
  int32_t x, y, z;
};

struct DoubleVector3 {
  double_t x, y, z;

  DoubleVector3(): x(0.0), y(0.0), z(0.0) {}

  DoubleVector3(double_t v): x(v), y(v), z(v) {}

  DoubleVector3(double_t _x, double_t _y, double_t _z): x(_x), y(_y), z(_z) {}

  DoubleVector3(double_t _x, double_t _z): x(_x), y(0.0), z(_z) {}

  double_t distanceToNoHeight(const DoubleVector3& ovec) const {
    const DoubleVector3 diff(x - ovec.x, z - ovec.z);
    return std::sqrt(diff.x * diff.x + diff.z * diff.z);
  }

  double_t distanceTo(const DoubleVector3& ovec) const {
    const DoubleVector3 diff(x - ovec.x, y - ovec.y, z - ovec.z);
    return std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
  }

  DoubleVector3 operator+(const DoubleVector3& b) const { return DoubleVector3(x + b.x, y + b.y, z + b.z); }

  DoubleVector3 operator-(const DoubleVector3& b) const { return DoubleVector3(x - b.x, y - b.y, z - b.z); }

  DoubleVector3 operator*(const DoubleVector3& b) const { return DoubleVector3(x * b.x, y * b.y, z * b.z); }

  DoubleVector3 operator*(double_t b) const { return DoubleVector3(x * b, y * b, z * b); }

  DoubleVector3& operator+=(const DoubleVector3& b) {
    x += b.x, y += b.y, z += b.z;
    return *this;
  }
};

struct FloatAngle {
  float_t yaw, pitch;

  inline int8_t yawToByte() const { return (yaw / 360) * 255; }

  inline int8_t pitchToByte() const { return (pitch / 360) * 255; }
};

struct FloatVector3 {
  float_t x, y, z;
};

struct VsDamageInfo {
  int16_t  damage;
  double_t kbackStrength;

  VsDamageInfo(): damage(1), kbackStrength(0.1) {}

  VsDamageInfo(int16_t dmg, double_t kb): damage(dmg), kbackStrength(kb) {}
};

enum Dimension : int8_t {
  Overworld = 0,
  Nether    = -1,
};

enum ArmorType : int8_t {
  Head,
  Chest,
  Pants,
  Boots,
};

namespace Helper {
bool stricmp(const auto& a, const auto& b) {
  return std::ranges::equal(a, b, [](auto a, auto b) { return std::tolower(a) == std::tolower(b); });
}

bool strcmp(const auto& a, const auto& b) {
  return std::ranges::equal(a, b);
}
} // namespace Helper
