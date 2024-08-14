#pragma once

#include "../ids.h"
#include "../packet.h"
#include "containers/list/playerContainer.h"
#include "helper.h"

namespace Packet {
#ifdef M173_ACTIVATE_READER_API
namespace FromClient {
class CloseWindow: private PacketReader {
  public:
  CloseWindow(SafeSocket& sock): PacketReader(sock) { m_win = readInteger<WinId>(); }

  const auto& getWindow() const { return m_win; }

  private:
  WinId m_win;
};

class ClickWindow: private PacketReader {
  public:
  ClickWindow(SafeSocket& sock): PacketReader(sock) {
    m_wid = readInteger<WinId>();                       // Window ID
    m_sid = readInteger<SlotId>();                      // Slot number
    m_rmb = readBoolean();                              // Is right click
    m_trc = readInteger<int16_t>();                     // Action number
    m_shf = readBoolean();                              // Is shift pressed
    if ((m_iis.itemId = readInteger<ItemId>()) != -1) { // Item ID
      m_iis.stackSize  = readInteger<int8_t>();         // Item count
      m_iis.itemDamage = readInteger<int16_t>();        // Item uses
    }
  }

  bool isInventory() const { return m_wid == 0; }

  bool isRightButton() const { return m_rmb; }

  bool isShift() const { return m_shf; }

  auto getSlot() const { return m_sid; }

  auto getWindow() const { return m_wid; }

  auto getItem() const { return m_iis; }

  auto getTransactionId() const { return m_trc; }

  private:
  WinId     m_wid;
  SlotId    m_sid;
  bool      m_rmb;
  bool      m_shf;
  int16_t   m_trc;
  ItemStack m_iis;
};

class TransactionWindow: private PacketReader {
  public:
  TransactionWindow(SafeSocket& sock): PacketReader(sock) {
    m_wid = readInteger<WinId>();   // Window ID
    m_trc = readInteger<int16_t>(); // Action number
    m_acp = readBoolean();          // Is accepted
  }

  bool isAccepted() const { return m_acp; }

  auto getWindow() const { return m_wid; }

  auto getTransactionId() const { return m_trc; }

  private:
  WinId   m_wid;
  bool    m_acp;
  int16_t m_trc;
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
  ItemsWindow(WinId wid, int16_t icount = 0): PacketWriter(Packet::IDs::ItemsWindow, 6 + icount * 5) {
    writeInteger<WinId>(wid);
    writeInteger<int16_t>(m_count = 0);
  }

  ItemsWindow(PlayerContainer& cont): ItemsWindow(0, cont.getSize()) {
    for (SlotId i = 0; i < cont.getSize(); ++i) {
      addItem(cont.getItem(i));
    }
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
