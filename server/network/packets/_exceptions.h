#pragma once

#include <exception>
#include <format>
#include <string>

class InvalidProtoException: public std::exception {
  public:
  InvalidProtoException(int32_t cver, int32_t ever) { m_what = std::format("Got unsupported protocol version (ex: {}, got: {})", ever, cver); }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

class InvalidNameException: public std::exception {
  public:
  enum Reason {
    NameTooLong,
    ProhibitSymbols,
  };

  InvalidNameException(Reason r, uint32_t add = 0) {
    switch (r) {
      case NameTooLong: {
        m_what = std::format("Your name is {} symbols long, 16 is maximum allowed!", add);
      } break;
      case ProhibitSymbols: {
        m_what = "Your name contains prohibited symbols!";
      } break;
    }
  }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};
