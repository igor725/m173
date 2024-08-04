#pragma once

#include "../ids.h"
#include "../packet.h"

namespace Packet {
namespace FromClient {
class Respawn: public PacketReader {
  public:
  Respawn(sockpp::tcp_socket& sock): PacketReader(sock) {
    m_dim = readInteger<Dimension>(); // Dimension
  }

  const auto getDimension() const { return m_dim; }

  private:
  Dimension m_dim;
};
} // namespace FromClient

namespace ToClient {
class Respawn: public PacketWriter {
  public:
  Respawn(Dimension dim): PacketWriter(Packet::IDs::Respawn) { writeInteger<Dimension>(dim); }
};
} // namespace ToClient
} // namespace Packet
