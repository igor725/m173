#define M173_ACTIVATE_READER_API
#include "Handshake.h"
#include "Player.h"
#include "_exceptions.h"

namespace {
void testProtoVer(int32_t proto) {
  constexpr int32_t SV_PROTO_VER = 14;
  if (proto != SV_PROTO_VER) throw InvalidProtoException(proto, SV_PROTO_VER);
}

void testUserName(std::wstring_view name) {
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
  m_id = 0;
}

LoginRequest::LoginRequest(SafeSocket& sock): PacketReader(sock) {
  m_protover = readInteger<int32_t>();
  testProtoVer(m_protover);

  readString(m_name);
  readInteger<int64_t>(); // Seed, just skipping the thing
  readInteger<int8_t>();  // Dimension, skipping too

  testUserName(m_name);
}

Respawn::Respawn(SafeSocket& sock): PacketReader(sock) {
  readInteger<Dimension>(); // Dimension
}
} // namespace FromClient

namespace ToClient {
KeepAlive::KeepAlive(uint32_t id): PacketWriter(Packet::IDs::KeepAlive, 0) {}

LoginResponse::LoginResponse(Entities::PlayerBase* pbase, IWorld& world, int8_t maxp): PacketWriter(Packet::IDs::Login, 15 + world.getName().length()) {
  writeInteger<EntityId>(pbase->getEntityId());
  writeString(world.getName());
  writeInteger<int64_t>(world.getSeed());
  writeInteger<int8_t>(pbase->getDimension());
}

Handshake::Handshake(std::wstring_view connhash): PacketWriter(Packet::IDs::Handshake, 2 + connhash.length()) {
  writeString(connhash);
}

PlayerHealth::PlayerHealth(Entities::PlayerBase* player): PacketWriter(Packet::IDs::PlayerHealth, 2) {
  writeInteger<int16_t>(player->getHealth());
}

PlayerRespawn::PlayerRespawn(Entities::PlayerBase* player, IWorld& world): PacketWriter(Packet::IDs::PlayerRespawn, 1) {
  writeInteger<Dimension>(player->getDimension());
}
} // namespace ToClient
} // namespace Packet
