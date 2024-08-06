#pragma once

#include "../ids.h"
#include "../packet.h"

namespace Packet {
namespace FromClient {
class Respawn: private PacketReader {
  public:
  Respawn(SafeSocket& sock): PacketReader(sock) {
    m_dim = readInteger<Dimension>(); // Dimension
  }

  const auto getDimension() const { return m_dim; }

  private:
  Dimension m_dim;
};
} // namespace FromClient

namespace ToClient {
class Respawn: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  Respawn(Dimension dim): PacketWriter(Packet::IDs::Respawn) { writeInteger<Dimension>(dim); }
};
} // namespace ToClient
} // namespace Packet
