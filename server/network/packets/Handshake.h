#pragma once

#include "../ids.h"
#include "../packet.h"

namespace Packet {
#ifdef M173_ACTIVATE_READER_API
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
#endif

namespace ToClient {
class KeepAlive: public PacketWriter {
  public:
  KeepAlive(): PacketWriter(Packet::IDs::KeepAlive, 0) {}
};

class LoginRequest: public PacketWriter {
  public:
  LoginRequest(EntityId entId, const std::wstring& svname, int8_t dimension): PacketWriter(Packet::IDs::Login, 15 + svname.size()) {
    writeInteger<EntityId>(entId);
    writeString(svname);
    writeInteger<int64_t>(0ll); // todo world seed
    writeInteger<int8_t>(dimension);
  }
};

class Handshake: public PacketWriter {
  public:
  Handshake(const std::wstring& connhash): PacketWriter(Packet::IDs::Handshake, 2 + connhash.size()) { writeString(connhash); }
};
} // namespace ToClient
} // namespace Packet
