#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class TimeUpdate: public PacketWriter {
  public:
  TimeUpdate(int64_t time): PacketWriter(Packet::IDs::TimeUpdate) { writeInteger<int64_t>(time); }
};

class SpawnPosition: public PacketWriter {
  public:
  SpawnPosition(const IntVector3& pos): PacketWriter(Packet::IDs::SpawnPos) {
    writeInteger<int32_t>(pos.x);
    writeInteger<int32_t>(pos.y);
    writeInteger<int32_t>(pos.z);
  }
};

class BlockChange: public PacketWriter {
  public:
  BlockChange(const IntVector3& pos, BlockId type, int8_t meta): PacketWriter(Packet::IDs::BlockChg) {
    writeInteger<int32_t>(pos.x);
    writeInteger<int8_t>(pos.y);
    writeInteger<int32_t>(pos.z);
    writeInteger<BlockId>(type);
    writeInteger<int8_t>(meta);
  }
};

class PreChunk: public PacketWriter {
  public:
  PreChunk(const IntVector2& pos, bool init): PacketWriter(Packet::IDs::PreChunk) {
    writeInteger<int32_t>(pos.x);
    writeInteger<int32_t>(pos.z);
    writeBoolean(init);
  }
};

class MapChunk: public PacketWriter {
  public:
  MapChunk(const IntVector3& pos, const ByteVector3& size, int32_t datasize): PacketWriter(Packet::IDs::MapChunk) {
    writeInteger<int32_t>(pos.x);
    writeInteger<int16_t>(pos.y);
    writeInteger<int32_t>(pos.z);
    writeInteger<int8_t>(size.x);
    writeInteger<int8_t>(size.y);
    writeInteger<int8_t>(size.z);
    writeInteger<int32_t>(datasize);
  }
};

class NoteBlockPlay: public PacketWriter {
  public:
  enum Instrument : int8_t {
    Harp,
    DoubleBass,
    SnareDrum,
    Sticks,
    BassDrum,
  };

  NoteBlockPlay(const IntVector3& pos, Instrument instr, int8_t pitch): PacketWriter(Packet::IDs::BlockAction) {
    writeInteger<int32_t>(pos.x);
    writeInteger<int16_t>(pos.y);
    writeInteger<int32_t>(pos.z);
    writeInteger<Instrument>(instr);
    writeInteger<int8_t>(pitch);
  }
};

class SetState: public PacketWriter {
  public:
  enum State : int8_t {
    InvalidBed,
    RainStart,
    RainEnd,
  };

  SetState(State s): PacketWriter(Packet::IDs::SetState) { writeInteger<State>(s); }
};

class SignUpdate: public PacketWriter {
  public:
  SignUpdate(const IntVector3& pos, const std::wstring& data);

  private:
  int32_t m_lineCount;
};
} // namespace ToClient

namespace FromClient {
class SignCreate: private PacketReader {
  public:
  SignCreate(SafeSocket& sock): PacketReader(sock) {
    m_pos.x = readInteger<int32_t>();
    m_pos.y = readInteger<int16_t>();
    m_pos.z = readInteger<int32_t>();

    for (int32_t i = 0; i < 4; ++i) {
      std::wstring temp;
      readString(temp);
      m_lines += temp;
      if (i != 3) m_lines.push_back(L'\n');
    }
  }

  private:
  IntVector3   m_pos;
  std::wstring m_lines;
};
} // namespace FromClient
} // namespace Packet
