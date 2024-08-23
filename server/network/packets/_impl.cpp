#define M173_ACTIVATE_READER_API
#include "Handshake.h"
#include "Window.h"
#include "World.h"
#include "world/chunkCompression.h"
#include "zlibpp/zlibpp_unique.h"

#include <exception>
#include <format>
#include <spdlog/spdlog.h>
#include <sstream>

#pragma region("packet.h/PacketWriter")

PacketWriter::PacketWriter(PacketId id, int32_t psize) {
  spdlog::trace("Writing packet {:02x} with known size {}", id, psize);
  m_data.reserve(psize + 1);
  writeInteger<int8_t>(id);
}

PacketWriter::PacketWriter(PacketId id) {
  m_data.reserve(1);
  spdlog::trace("Writing packet {:02x} with unknown size!", id);
  writeInteger<int8_t>(id);
}

#pragma endregion

#pragma region("Window.h")

namespace Packet::ToClient {
OpenWindow::OpenWindow(UiWindow* win): PacketWriter(Packet::IDs::NewWindow) {
  writeInteger<WinId>(win->getId());
  writeInteger<UiWindow::Type>(win->getType());
  writeString(std::string_view(win->getName()));
  writeInteger<int8_t>(win->container()->getSize());
}
} // namespace Packet::ToClient

#pragma endregion

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

MapChunk::MapChunk(const IntVector3& pos, const ByteVector3& size, const ChunkUnique& chunk): PacketWriter(Packet::IDs::MapChunk, 512) {
  writeInteger<int32_t>(pos.x);
  writeInteger<int16_t>(pos.y);
  writeInteger<int32_t>(pos.z);
  writeInteger<int8_t>(size.x);
  writeInteger<int8_t>(size.y);
  writeInteger<int8_t>(size.z);
  auto csize_pos = m_data.size();
  writeInteger<int32_t>(0);

  std::array<char, 256> buffer;

  bool compr_done = false;

  auto worker = cmp.acquire();

  ChunkZlib ccomp(worker, chunk, ChunkZlib::Type::Compressor);

  do {
    ccomp.feed();

    switch (worker->tick()) {
      case IZLibPP::MoreOut: {
        worker->setOutput(buffer.data(), buffer.size());
      } break;
      case IZLibPP::Done: {
        compr_done = true;
      } break;

      default: break;
    }

    m_data.insert(m_data.end(), buffer.begin(), buffer.begin() + worker->getFrameSize());
    worker->setOutput(buffer.data(), buffer.size());
  } while (!compr_done);

  *reinterpret_cast<uint32_t*>(m_data.data() + csize_pos) = bswap(static_cast<uint32_t>(worker->getTotalOutput()));
}
} // namespace Packet::ToClient

#pragma endregion()

#pragma region("Handshake.h")

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
        m_what = "Your name contains prohibited symbols!";
      } break;
    }
  }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

namespace Packet::FromClient {
SignCreate::SignCreate(SafeSocket& sock): PacketReader(sock) {
  m_lines.reserve(64);

  m_pos.x = readInteger<int32_t>();
  m_pos.y = readInteger<int16_t>();
  m_pos.z = readInteger<int32_t>();

  for (int32_t i = 0; i < 4; ++i) {
    std::wstring temp;
    temp.reserve(15);

    readString(temp);
    m_lines += temp;
    if (i != 3) m_lines.push_back(L'\n');
  }
}

void LoginRequest::testProtoVer(int32_t proto) {
  constexpr int32_t SV_PROTO_VER = 14;
  if (proto != SV_PROTO_VER) throw InvalidProtoException(proto, SV_PROTO_VER);
}

void LoginRequest::testUserName(const std::wstring_view name) {
  auto testSym = [](wchar_t sym) -> bool {
    return (sym >= '0' && sym <= L'9') || (sym >= L'A' && sym <= L'Z') || (sym >= L'a' && sym <= L'z') || (sym == L'_');
  };

  const auto nameLen = name.size();
  if (nameLen > 16) throw InvalidNameException(InvalidNameException::NameTooLong, nameLen);
  if (std::find_if_not(name.begin(), name.end(), testSym) != name.end()) throw InvalidNameException(InvalidNameException::ProhibitSymbols, nameLen);
}
} // namespace Packet::FromClient

#pragma endregion()
