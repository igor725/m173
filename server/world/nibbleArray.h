#pragma once

#include "helper.h"

#include <cstddef>

struct Nibble {
  int8_t a : 4;
  int8_t b : 4;

  Nibble(): a(0), b(0) {}

  Nibble(int8_t _x): a(_x), b(_x) {}

  Nibble(int8_t _a, int8_t _b): a(_a), b(_b) {}
};

template <std::size_t N>
class NibbleArray {
  public:
  NibbleArray(): m_data({0}) {}

  int8_t getNibble(uint32_t blockIndex) const { return (blockIndex & 1) == 0 ? m_data[blockIndex >> 1].a : m_data[blockIndex >> 1].b; }

  void setNibble(uint32_t blockIndex, int8_t value) {
    Nibble& nibb = m_data[blockIndex >> 1];
    nibb         = (blockIndex & 1) == 0 ? Nibble(value, nibb.b) : Nibble(nibb.a, value);
  }

  auto data() { return m_data.data(); }

  auto size() const { return m_data.size(); }

  void fill(const Nibble& nib) { m_data.fill(nib); }

  private:
  std::array<Nibble, (N >> 1)> m_data;
};
