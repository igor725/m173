#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class TimeUpdate: public PacketWriter {
  public:
  TimeUpdate(int64_t time): PacketWriter(Packet::IDs::TimeUpdate) { writeInteger(time); }
};

class SpawnPosition: public PacketWriter {
  public:
  SpawnPosition(const IntVector3& pos): PacketWriter(Packet::IDs::SpawnPos) {
    writeInteger(pos.x);
    writeInteger(pos.y);
    writeInteger(pos.z);
  }
};

class BlockChange: public PacketWriter {
  public:
  BlockChange(const IntVector3& pos, BlockId type, int8_t meta): PacketWriter(Packet::IDs::BlockChg) {
    writeInteger(pos.x);
    writeInteger<int8_t>(pos.y);
    writeInteger(pos.z);
    writeInteger(type);
    writeInteger(meta);
  }
};

class PreChunk: public PacketWriter {
  public:
  PreChunk(const IntVector2& pos, bool init): PacketWriter(Packet::IDs::PreChunk) {
    writeInteger(pos.x);
    writeInteger(pos.z);
    writeBoolean(init);
  }
};

class MapChunk: public PacketWriter {
  public:
  MapChunk(const IntVector3& pos, const ByteVector3& size, int32_t datasize): PacketWriter(Packet::IDs::MapChunk) {
    writeInteger(pos.x);
    writeInteger<int16_t>(pos.y);
    writeInteger(pos.z);
    writeInteger(size.x);
    writeInteger(size.y);
    writeInteger(size.z);
    writeInteger(datasize);
  }
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
