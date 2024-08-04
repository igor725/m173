#pragma once

#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace FromClient {
class CloseWindow: private PacketReader {
  public:
  CloseWindow(sockpp::tcp_socket& sock): PacketReader(sock) { m_win = readInteger<WinId>(); }

  const auto& getWindowId() const { return m_win; }

  private:
  WinId m_win;
};
} // namespace FromClient

namespace ToClient {
class OpenWindow: public PacketWriter {
  public:
  OpenWindow(WinId wid, InvId iid, std::string& name, int8_t numslots): PacketWriter(0x64) {
    writeInteger(wid);
    writeInteger(iid);
    writeString(name);
    writeInteger(numslots);
  }
};

class CloseWindow: public PacketWriter {
  public:
  CloseWindow(WinId wid): PacketWriter(0x65) { writeInteger(wid); }
};
} // namespace ToClient
} // namespace Packet
