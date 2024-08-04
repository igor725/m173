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

class ClickWindow: private PacketReader {
  public:
  ClickWindow(sockpp::tcp_socket& sock): PacketReader(sock) {
    readInteger<WinId>();                        // Window ID
    readInteger<int16_t>();                      // Slot number
    readBoolean();                               // Is right click
    readInteger<int16_t>();                      // Action number
    readBoolean();                               // Is shift pressed
    if ((m_iid = readInteger<ItemId>()) != -1) { // Item ID
      readInteger<int8_t>();                     // Item count
      readInteger<int16_t>();                    // Item uses
    }
  }

  private:
  ItemId m_iid;
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
