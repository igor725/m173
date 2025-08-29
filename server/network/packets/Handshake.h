#pragma once

#include "../ids.h"
#include "../packet.h"
#include "world/world.h"

#include <string_view>

namespace Packet {
#ifdef M173_ACTIVATE_READER_API
namespace FromClient {
class KeepAlive: private PacketReader {
  public:
  KeepAlive(SafeSocket& sock);

  uint32_t getId() const { return m_id; }

  private:
  uint32_t m_id;
};

class LoginRequest: private PacketReader {
  public:
  LoginRequest(SafeSocket& sock);

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

class ServerPing: private PacketReader {
  public:
  ServerPing(SafeSocket& sock);
};
} // namespace FromClient
#endif

namespace ToClient {
class KeepAlive: public PacketWriter {
  public:
  KeepAlive(uint32_t id);
};

class LoginResponse: public PacketWriter {
  public:
  LoginResponse(Entities::PlayerBase* pbase, IWorld& world, int8_t maxp);
};

class Handshake: public PacketWriter {
  public:
  Handshake(const std::wstring_view connhash);
};
} // namespace ToClient
} // namespace Packet
