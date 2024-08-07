#pragma once

#include "../ids.h"
#include "../packet.h"

#include <exception>

class InvalidNameException: public std::exception {
  public:
  enum Reason {
    NameTooLong,
    ProhibitSymbols,
  };

  InvalidNameException(Reason r, uint32_t add) {
    switch (r) {
      case NameTooLong: {
        m_what = std::format("Your name is {} symbols long, 16 is maximum allowed!", add);
      } break;
      case ProhibitSymbols: {
        m_what = std::format("Your name contains {} prohibited symbols!", add);
      } break;
    }
  }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

static inline void testUserName(const std::wstring& name) {
  const auto nameLen = name.size();
  if (nameLen > 16) throw InvalidNameException(InvalidNameException::NameTooLong, nameLen);
  // todo prohibited symbols test
}

namespace Packet {
namespace FromClient {
class LoginRequest: private PacketReader {
  public:
  LoginRequest(SafeSocket& sock): PacketReader(sock) {
    m_protover = readInteger<int32_t>();
    readString(m_name);
    readInteger<int64_t>(); // Seed, just skipping the thing
    readInteger<int8_t>();  // Dimension, skipping too

    testUserName(m_name);
  }

  auto& getName() const { return m_name; }

  private:
  int32_t      m_protover;
  std::wstring m_name;
};

class Handshake: private PacketReader {
  public:
  Handshake(SafeSocket& sock): PacketReader(sock) {
    readString(m_name_or_hash);
    // We don't really care about the name user sent us there
  }

  const auto& getName() const { return m_name_or_hash; }

  private:
  std::wstring m_name_or_hash;
};
} // namespace FromClient

namespace ToClient {
class LoginRequest: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  LoginRequest(int32_t entId, const std::wstring& svname, int8_t dimension): PacketWriter(Packet::IDs::Login) {
    writeInteger(entId);
    writeString(svname);
    writeInteger(0ll); // todo world seed
    writeInteger(dimension);
  }
};

class Handshake: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  Handshake(const std::wstring& connhash): PacketWriter(Packet::IDs::Handshake) { writeString(connhash); }
};
} // namespace ToClient
} // namespace Packet
