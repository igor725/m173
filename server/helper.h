#pragma once

#include <cmath>
#include <cstdint>

#define CLASS_NO_COPY(name)                                                                                                                                    \
  name(const name&)            = delete;                                                                                                                       \
  name& operator=(const name&) = delete;

#define CLASS_NO_MOVE(name)                                                                                                                                    \
  name(name&&) noexcept            = delete;                                                                                                                   \
  name& operator=(name&&) noexcept = delete

typedef int32_t EntityId;
typedef int16_t ItemId;
typedef int8_t  AnimId;
typedef int8_t  WinId;
typedef int8_t  InvId;

struct IntVector2 {
  int32_t x, z;
};

struct ByteVector3 {
  int8_t x, y, z;
};

struct IntVector3 {
  int32_t x, y, z;
};

struct DoubleVector3 {
  double_t x, y, z;
};

struct FloatVector3 {
  float_t x, y, z;
};
