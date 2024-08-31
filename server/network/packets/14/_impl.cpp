#define M173_ACTIVATE_READER_API
#include "../_exceptions.h"
#include "Handshake.h"

#pragma region("Handshake.h")

namespace Packet::FromClient {
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
  if (std::find_if_not(name.begin(), name.end(), testSym) != name.end()) throw InvalidNameException(InvalidNameException::ProhibitSymbols);
}
} // namespace Packet::FromClient

#pragma endregion()
