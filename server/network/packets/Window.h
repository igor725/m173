#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"

namespace Packet {
#ifdef M173_ACTIVATE_READER_API
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
    readInteger<SlotId>();                       // Slot number
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
#endif

namespace ToClient {
class OpenWindow: public PacketWriter {
  public:
  OpenWindow(WinId wid, InvId iid, const std::string& name, int8_t numslots): PacketWriter(Packet::IDs::NewWindow) {
    writeInteger<WinId>(wid);
    writeString(name);
    writeInteger<InvId>(iid);
    writeInteger<int16_t>(numslots);
  }
};

class CloseWindow: public PacketWriter {
  public:
  CloseWindow(WinId wid): PacketWriter(Packet::IDs::CloseWindow) { writeInteger<WinId>(wid); }
};

class SetSlotWindow: public PacketWriter {
  public:
  SetSlotWindow(WinId wid, SlotId slot, const ItemStack& is): PacketWriter(Packet::IDs::SetSlot) {
    writeInteger<WinId>(wid);
    writeInteger<SlotId>(slot);
    writeInteger<ItemId>(is.itemId);
    if (is.itemId != -1) {
      writeInteger<int8_t>(is.stackSize);
      writeInteger<int16_t>(is.itemDamage);
    }
  }
};

class ItemsWindow: public PacketWriter {
  public:
  ItemsWindow(WinId wid): PacketWriter(Packet::IDs::ItemsWindow) {
    writeInteger<WinId>(wid);
    writeInteger<int16_t>(m_count = 0);
  }

  void addItem(const ItemStack& is) {
    (*(reinterpret_cast<int16_t*>(m_data.data() + 2))) = bswap(++m_count);
    writeInteger<ItemId>(is.itemId);
    if (is.itemId >= 0) {
      writeInteger<int8_t>(is.stackSize);
      writeInteger<int16_t>(is.itemDamage);
    }
  }

  private:
  int16_t m_count;
};

class UpdateWindow: public PacketWriter {
  public:
  UpdateWindow(WinId wid, int16_t type, int16_t value): PacketWriter(Packet::IDs::UpdateWindow, 5) {
    writeInteger<WinId>(wid);
    writeInteger<int16_t>(type);
    writeInteger<int16_t>(value);
  }
};

class TransactionWindow: public PacketWriter {
  public:
  TransactionWindow(WinId wid, int16_t anum, bool accepted): PacketWriter(Packet::IDs::TransactWindow, 4) {
    writeInteger<WinId>(wid);
    writeInteger<int16_t>(anum);
    writeBoolean(accepted);
  }
};
} // namespace ToClient
} // namespace Packet
