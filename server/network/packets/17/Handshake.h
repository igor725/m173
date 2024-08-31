#pragma once

#include "../../ids.h"
#include "../../packet.h"
#include "entity/creatures/player.h"
#include "world/world.h"

namespace Packet {
#ifdef M173_ACTIVATE_READER_API
namespace FromClient {
class KeepAlive: private PacketReader {
  public:
  KeepAlive(SafeSocket& sock): PacketReader(sock) { m_id = readInteger<uint32_t>(); }

  uint32_t getId() const { return m_id; }

  private:
  uint32_t m_id;
};

class LoginRequest: private PacketReader {
  public:
  LoginRequest(SafeSocket& sock): PacketReader(sock) {
    m_protover = readInteger<int32_t>();
    readString(m_name);
    readInteger<int64_t>(); // Seed, just skipping the thing
    readInteger<int32_t>(); // ???
    readInteger<int8_t>();  // ???
    readInteger<int8_t>();  // ???
    readInteger<int8_t>();  // ???
    readInteger<int8_t>();  // ???

    testProtoVer(m_protover);
    testUserName(m_name);
  }

  auto& getName() const { return m_name; }

  private:
  void testProtoVer(int32_t proto);
  void testUserName(const std::wstring_view name);

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
  ServerPing(SafeSocket& sock): PacketReader(sock) {}
};
} // namespace FromClient
#endif

namespace ToClient {
class KeepAlive: public PacketWriter {
  public:
  KeepAlive(uint32_t id): PacketWriter(Packet::IDs::KeepAlive, 4) { writeInteger<uint32_t>(id); }
};

class LoginResponse: public PacketWriter {
  public:
  LoginResponse(PlayerBase* pbase, IWorld& world, int8_t maxp): PacketWriter(Packet::IDs::Login, 19 + world.getName().length()) {
    writeInteger<EntityId>(pbase->getEntityId());
    writeString(world.getName());
    writeInteger<int64_t>(world.getSeed());
    writeInteger<int32_t>(world.getGameType());
    writeInteger<int8_t>(pbase->getDimension());
    writeInteger<int8_t>(world.getDifficulty());
    writeInteger<int8_t>(-128); // Does not really used by client
    writeInteger<int8_t>(maxp);
  }
};

class Handshake: public PacketWriter {
  public:
  Handshake(const std::wstring_view connhash): PacketWriter(Packet::IDs::Handshake, 2 + connhash.length()) { writeString(connhash); }
};
} // namespace ToClient
} // namespace Packet
