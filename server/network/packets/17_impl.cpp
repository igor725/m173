#define M173_ACTIVATE_READER_API
#include "Handshake.h"
#include "Player.h"
#include "_exceptions.h"

namespace {
void testProtoVer(int32_t proto) {
  constexpr int32_t SV_PROTO_VER = 17;
  if (proto != SV_PROTO_VER) throw InvalidProtoException(proto, SV_PROTO_VER);
}

void testUserName(const std::wstring_view name) {
  auto testSym = [](wchar_t sym) -> bool {
    return (sym >= '0' && sym <= L'9') || (sym >= L'A' && sym <= L'Z') || (sym >= L'a' && sym <= L'z') || (sym == L'_');
  };

  const auto nameLen = name.size();
  if (nameLen > 16) throw InvalidNameException(InvalidNameException::NameTooLong, nameLen);
  if (std::find_if_not(name.begin(), name.end(), testSym) != name.end()) throw InvalidNameException(InvalidNameException::ProhibitSymbols);
}
} // namespace

namespace Packet {
namespace FromClient {
KeepAlive::KeepAlive(SafeSocket& sock): PacketReader(sock) {
  m_id = readInteger<uint32_t>();
}

LoginRequest::LoginRequest(SafeSocket& sock): PacketReader(sock) {
  m_protover = readInteger<int32_t>();
  testProtoVer(m_protover);

  readString(m_name);
  readInteger<int64_t>(); // Seed, just skipping the thing
  readInteger<int32_t>(); // ???
  readInteger<int8_t>();  // ???
  readInteger<int8_t>();  // ???
  readInteger<int8_t>();  // ???
  readInteger<int8_t>();  // ???

  testUserName(m_name);
}

Respawn::Respawn(SafeSocket& sock): PacketReader(sock) {
  readInteger<Dimension>(); // Dimension
  readInteger<int8_t>();    // Difficulty
  readInteger<int8_t>();    // Game type
  readInteger<int16_t>();   // Unused
  readInteger<int64_t>();   // Seed
}

ServerPing::ServerPing(SafeSocket& sock): PacketReader(sock) {}
} // namespace FromClient

namespace ToClient {
KeepAlive::KeepAlive(uint32_t id): PacketWriter(Packet::IDs::KeepAlive, 4) {
  writeInteger<uint32_t>(id);
}

LoginResponse::LoginResponse(PlayerBase* pbase, IWorld& world, int8_t maxp): PacketWriter(Packet::IDs::Login, 19 + world.getName().length()) {
  writeInteger<EntityId>(pbase->getEntityId());
  writeString(world.getName());
  writeInteger<int64_t>(world.getSeed());
  writeInteger<int32_t>(world.getGameType());
  writeInteger<int8_t>(pbase->getDimension());
  writeInteger<int8_t>(world.getDifficulty());
  writeInteger<int8_t>(-128); // Does not really used by client
  writeInteger<int8_t>(maxp);
}

Handshake::Handshake(const std::wstring_view connhash): PacketWriter(Packet::IDs::Handshake, 2 + connhash.length()) {
  writeString(connhash);
}

PlayerHealth::PlayerHealth(PlayerBase* player): PacketWriter(Packet::IDs::PlayerHealth, 8) {
  writeInteger<int16_t>(player->getHealth());
  writeInteger<int16_t>(player->getHunger());
  writeFloating<float_t>(player->getSatur());
}

PlayerRespawn::PlayerRespawn(PlayerBase* player, IWorld& world): PacketWriter(Packet::IDs::PlayerRespawn, 13) {
  writeInteger<Dimension>(player->getDimension());
  writeInteger<int8_t>(world.getDifficulty()); // Difficulty
  writeInteger<int8_t>(world.getGameType());   // Game type
  writeInteger<int16_t>(128);
  writeInteger<int64_t>(world.getSeed());
}
} // namespace ToClient
} // namespace Packet

#pragma endregion()
