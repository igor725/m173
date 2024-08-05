#pragma once

#include <exception>
#include <format>
#include <string>
#include <zlib.h>

class ZlibException: public std::exception {
  public:
  ZlibException(int32_t ret) { m_what = std::format("Zlib error: {}", zError(ret)); }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};
