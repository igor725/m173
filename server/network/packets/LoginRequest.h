#pragma once

#include "../ids.h"
#include "../packet.h"

namespace Packet {
namespace FromClient {
class LoginRequest: private PacketReader {
  public:
  LoginRequest(sockpp::tcp_socket& sock): PacketReader(sock) {
    m_protover = readInteger<int32_t>();
    readString(m_name);
    readInteger<int64_t>(); // Seed, just skipping the thing
    readInteger<int8_t>();  // Dimension, skipping too
  }

  private:
  int32_t      m_protover;
  std::wstring m_name;
};
} // namespace FromClient

namespace ToClient {
class LoginRequest: public PacketWriter {
  public:
  LoginRequest(int32_t entId, std::wstring& svname, int8_t dimension): PacketWriter(Packet::IDs::Login) {
    writeInteger(entId);
    writeString(svname);
    writeInteger(0ll); // todo world seed
    writeInteger(dimension);
  }
};
} // namespace ToClient
} // namespace Packet
