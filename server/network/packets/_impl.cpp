#define M173_ACTIVATE_READER_API
#include "Handshake.h"
#include "World.h"
#include "zlibpp/zlibpp_unique.h"

#include <exception>
#include <sstream>

#pragma region("World.h")

static UniqueZlibPP cmp(createCompressor());

class TooMuchSignLinesException: public std::exception {
  public:
  const char* what() const noexcept override { return "Your sign has too much lines!"; }
};

namespace Packet::ToClient {
SignUpdate::SignUpdate(const IntVector3& pos, const std::wstring& data): PacketWriter(Packet::IDs::SignUpdate, 10 + data.size()) {
  writeInteger<int32_t>(pos.x);
  writeInteger<int16_t>(pos.y);
  writeInteger<int32_t>(pos.z);

  std::wstringstream ss(data);
  std::wstring       temp;

  while (++m_lineCount < 5 && std::getline(ss, temp, L'\n')) {
    writeString(temp);
  }

  if (m_lineCount > 4) throw TooMuchSignLinesException();
}

MapChunk::MapChunk(const IntVector3& pos, const ByteVector3& size, Chunk* chunk): PacketWriter(Packet::IDs::MapChunk, 512) {
  writeInteger<int32_t>(pos.x);
  writeInteger<int16_t>(pos.y);
  writeInteger<int32_t>(pos.z);
  writeInteger<int8_t>(size.x);
  writeInteger<int8_t>(size.y);
  writeInteger<int8_t>(size.z);
  auto csize_pos = m_data.size();
  writeInteger<int32_t>(0);

  int32_t cstate = 0;

  std::array<char, 256> buffer;

  bool compr_done = false;

  auto acq   = cmp.acquire();
  auto compr = acq.get();

  do {
    if (compr->getAvailableInput() == 0) {
      switch (cstate++) {
        case 0: { // First things first, send the blocks array
          compr->setInput(chunk->m_blocks.data(), sizeof(chunk->m_blocks));
        } break;
        case 1: { // Now the meta for blocks
          compr->setInput(chunk->m_meta.data(), sizeof(chunk->m_meta));
        } break;
        case 2: { // Aaand block light array, whatever it means
          compr->setInput(chunk->m_light.data(), sizeof(chunk->m_light));
        } break;
        case 3: { // This one I don't even know
          compr->setInput(chunk->m_sky.data(), sizeof(chunk->m_sky));
        } break;
      }
    }

    switch (compr->tick()) {
      case IZLibPP::MoreOut: {
        compr->setOutput(buffer.data(), buffer.size());
      } break;
      case IZLibPP::Done: {
        compr_done = true;
      } break;
    }

    m_data.insert(m_data.end(), buffer.begin(), buffer.begin() + compr->getFrameSize());
    compr->setOutput(buffer.data(), buffer.size());
  } while (!compr_done);

  *reinterpret_cast<uint32_t*>(m_data.data() + csize_pos) = bswap(compr->getTotalOutput());
}
} // namespace Packet::ToClient

#pragma endregion()

#pragma region("Handshake.h")

#include <exception>
#include <format>

class InvalidProtoException: public std::exception {
  public:
  InvalidProtoException(int32_t cver, int32_t ever) { m_what = std::format("Got unsupported protocol version (ex: {}, got: {})", ever, cver); }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

class InvalidNameException: public std::exception {
  public:
  enum Reason {
    NameTooLong,
    ProhibitSymbols,
  };

  InvalidNameException(Reason r, uint32_t add) {
    switch (r) {
      case NameTooLong: {
        m_what = std::format("Your name is {} symbols long, 16 is maximum allowed!", add);
      } break;
      case ProhibitSymbols: {
        m_what = std::format("Your name contains {} prohibited symbols!", add);
      } break;
    }
  }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

namespace Packet::FromClient {
void LoginRequest::testProtoVer(int32_t proto) {
  constexpr int32_t SV_PROTO_VER = 14;
  if (proto != SV_PROTO_VER) throw InvalidProtoException(proto, SV_PROTO_VER);
}

void LoginRequest::testUserName(const std::wstring& name) {
  const auto nameLen = name.size();
  if (nameLen > 16) throw InvalidNameException(InvalidNameException::NameTooLong, nameLen);
  // todo prohibited symbols test
}
} // namespace Packet::FromClient

#pragma endregion()
