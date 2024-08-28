#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"
#include "world/world.h"

namespace Packet {
#ifdef M173_ACTIVATE_READER_API
namespace FromClient {
class SignCreate: private PacketReader {
  public:
  SignCreate(SafeSocket& sock);

  auto& getPosition() const { return m_pos; }

  private:
  IntVector3   m_pos;
  std::wstring m_lines;
};
} // namespace FromClient
#endif

namespace ToClient {
class TimeUpdate: public PacketWriter {
  public:
  TimeUpdate(int64_t time): PacketWriter(Packet::IDs::TimeUpdate, 8) { writeInteger<int64_t>(time); }
};

class SpawnPosition: public PacketWriter {
  public:
  SpawnPosition(const IntVector3& pos): PacketWriter(Packet::IDs::SpawnPos, 12) { writeIVector(pos); }
};

class BlockChange: public PacketWriter {
  public:
  BlockChange(const IntVector3& pos, BlockId type, int8_t meta): PacketWriter(Packet::IDs::BlockChg, 11) {
    writeInteger<int32_t>(pos.x);
    writeInteger<int8_t>(pos.y);
    writeInteger<int32_t>(pos.z);
    writeInteger<BlockId>(type);
    writeInteger<int8_t>(meta);
  }
};

class PreChunk: public PacketWriter {
  public:
  PreChunk(const IntVector2& pos, bool init): PacketWriter(Packet::IDs::PreChunk, 9) {
    writeInteger<int32_t>(pos.x);
    writeInteger<int32_t>(pos.z);
    writeBoolean(init);
  }
};

class MapChunk: public PacketWriter {
  public:
  MapChunk(const IntVector3& pos, const ByteVector3& size, const ChunkUnique& chunk);

  MapChunk(const ChunkUnique& chunk): MapChunk(chunk->getStartBlock(), CHUNK_DIMS, chunk) {}
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

  NoteBlockPlay(const IntVector3& pos, Instrument instr, int8_t pitch): PacketWriter(Packet::IDs::BlockAction, 12) {
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

  SetState(State s): PacketWriter(Packet::IDs::SetState, 1) { writeInteger<State>(s); }
};

class SignUpdate: public PacketWriter {
  public:
  SignUpdate(const IntVector3& pos, const std::wstring& data);

  private:
  int32_t m_lineCount;
};
} // namespace ToClient
} // namespace Packet
