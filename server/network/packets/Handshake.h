#pragma once

#include "../ids.h"
#include "../packet.h"

namespace Packet {
namespace FromClient {
class LoginRequest: private PacketReader {
  public:
  LoginRequest(SafeSocket& sock): PacketReader(sock) {
    m_protover = readInteger<int32_t>();
    readString(m_name);
    readInteger<int64_t>(); // Seed, just skipping the thing
    readInteger<int8_t>();  // Dimension, skipping too

    testProtoVer(m_protover);
    testUserName(m_name);
  }

  auto& getName() const { return m_name; }

  private:
  void testProtoVer(int32_t proto);
  void testUserName(const std::wstring& name);

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
class KeepAlive: public PacketWriter {
  public:
  KeepAlive(): PacketWriter(Packet::IDs::KeepAlive) {}
};

class LoginRequest: public PacketWriter {
  public:
  LoginRequest(int32_t entId, const std::wstring& svname, int8_t dimension): PacketWriter(Packet::IDs::Login) {
    writeInteger(entId);
    writeString(svname);
    writeInteger(0ll); // todo world seed
    writeInteger(dimension);
  }
};

class Handshake: public PacketWriter {
  public:
  Handshake(const std::wstring& connhash): PacketWriter(Packet::IDs::Handshake) { writeString(connhash); }
};
} // namespace ToClient
} // namespace Packet
