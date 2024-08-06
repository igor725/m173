#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace FromClient {
class CloseWindow: private PacketReader {
  public:
  CloseWindow(SafeSocket& sock): PacketReader(sock) { m_win = readInteger<WinId>(); }

  const auto& getWindowId() const { return m_win; }

  private:
  WinId m_win;
};

class ClickWindow: private PacketReader {
  public:
  ClickWindow(SafeSocket& sock): PacketReader(sock) {
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
class OpenWindow: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  OpenWindow(WinId wid, InvId iid, const std::string& name, int8_t numslots): PacketWriter(Packet::IDs::NewWindow) {
    writeInteger(wid);
    writeInteger(iid);
    writeString(name);
    writeInteger(numslots);
  }
};

class CloseWindow: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  CloseWindow(WinId wid): PacketWriter(Packet::IDs::CloseWindow) { writeInteger(wid); }
};

class UpdateWindow: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  UpdateWindow(WinId wid, int16_t type, int16_t value): PacketWriter(Packet::IDs::UpdateWindow) {
    writeInteger(wid);
    writeInteger(type);
    writeInteger(value);
  }
};

class TransactionWindow: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  TransactionWindow(WinId wid, int16_t anum, bool accepted): PacketWriter(Packet::IDs::TransactWindow) {
    writeInteger(wid);
    writeInteger(anum);
    writeBoolean(accepted);
  }
};
} // namespace ToClient
} // namespace Packet
