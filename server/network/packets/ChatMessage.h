#pragma once

#include "../ids.h"
#include "../packet.h"

namespace Packet {
namespace FromClient {
class ChatMessage: public PacketReader {
  public:
  ChatMessage(sockpp::tcp_socket& sock): PacketReader(sock) { readString(m_message); }

  private:
  std::wstring m_message;
};
} // namespace FromClient

namespace ToClient {
class ChatMessage: public PacketWriter {
  public:
  ChatMessage(std::wstring& message): PacketWriter(Packet::IDs::ChatMessage) { writeString(message); }
};
} // namespace ToClient
} // namespace Packet
