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
typedef uint8_t PacketId;

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

enum Dimension : int8_t {
  Overworld = 0,
  Nether    = -1,
};

enum DigStatus : int8_t {
  Started  = 0,
  Finished = 2,
  DropItem = 4,
};

enum SlotId : int8_t {
  CraftResult,
  CraftSlot_0,
  CraftSlot_1,
  CraftSlot_2,
  CraftSlot_3,
  ArmorHead,
  ArmorChest,
  ArmorPants,
  ArmorLegs,
  InvSlot_1x1,
  InvSlot_1x2,
  InvSlot_1x3,
  InvSlot_1x4,
  InvSlot_1x5,
  InvSlot_1x6,
  InvSlot_1x7,
  InvSlot_1x8,
  InvSlot_1x9,
  InvSlot_2x1,
  InvSlot_2x2,
  InvSlot_2x3,
  InvSlot_2x4,
  InvSlot_2x5,
  InvSlot_2x6,
  InvSlot_2x7,
  InvSlot_2x8,
  InvSlot_2x9,
  InvSlot_3x1,
  InvSlot_3x2,
  InvSlot_3x3,
  InvSlot_3x4,
  InvSlot_3x5,
  InvSlot_3x6,
  InvSlot_3x7,
  InvSlot_3x8,
  InvSlot_3x9,
  Hotbar_0,
  Hotbar_1,
  Hotbar_2,
  Hotbar_3,
  Hotbar_4,
  Hotbar_5,
  Hotbar_6,
  Hotbar_7,
  Hotbar_8,
};
