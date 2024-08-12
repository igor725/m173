#pragma once

#include <exception>
#include <format>
#include <string>
#include <zlib.h>

class ZlibException: public std::exception {
  public:
  ZlibException(int32_t ret): m_code(ret) { m_what = std::format("zlib| {}", zError(ret)); }

  const char* what() const noexcept override { return m_what.c_str(); }

  int32_t getCode() const noexcept { return m_code; }

  private:
  std::string m_what;
  int32_t     m_code;
};

class ZlibNotEnoughSpaceException: public std::exception {
  public:
  ZlibNotEnoughSpaceException() {}

  const char* what() const noexcept override { return "Not enough output space"; }
};
