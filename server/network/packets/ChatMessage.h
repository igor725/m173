#pragma once

#include "../ids.h"
#include "../packet.h"

namespace Packet {
#ifdef M173_ACTIVATE_READER_API
namespace FromClient {
class ChatMessage: private PacketReader {
  public:
  ChatMessage(SafeSocket& sock): PacketReader(sock) { readString(m_message); }

  auto& getMessage() { return m_message; }

  private:
  std::wstring m_message;
};
} // namespace FromClient
#endif

namespace ToClient {
class ChatMessage: public PacketWriter {
  public:
  ChatMessage(std::wstring_view message): PacketWriter(Packet::IDs::ChatMessage, message.size() + 2) { writeString(message); }
};
} // namespace ToClient
} // namespace Packet
