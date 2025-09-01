#include <cstdint>
#include <string>
#include <string_view>

namespace Helper {
std::wstring cvtToUCS2(std::string_view str) {
  std::wstring wtext;
  size_t       i = 0;
  while (i < str.size()) {
    uint32_t      codepoint = 0;
    unsigned char c         = str[i];

    if (c <= 0x7F) { // ASCII
      codepoint = c;
      i += 1;
    } else if ((c & 0xE0) == 0xC0) { // 2-byte sequence
      if (i + 1 >= str.size()) goto invaid_cp;
      codepoint = ((c & 0x1F) << 6) | (str[i + 1] & 0x3F);
      i += 2;
    } else if ((c & 0xF0) == 0xE0) { // 3-byte sequence
      if (i + 2 >= str.size()) goto invaid_cp;
      codepoint = ((c & 0x0F) << 12) | ((str[i + 1] & 0x3F) << 6) | (str[i + 2] & 0x3F);
      i += 3;
    } else if ((c & 0xF8) == 0xF0) { // 4-byte sequence? Can't convert to UCS-2
      goto invaid_cp;
    } else { // Corrupted UTF-8 sequence
      goto invaid_cp;
    }

    if (codepoint > 0xFFFF) { // Out of range codepoint
      goto invaid_cp;
    }

    wtext.push_back(static_cast<char16_t>(codepoint));
    continue;

  invaid_cp:
    i += 1;
    wtext.push_back(L'?');
  }

  return wtext;
}

std::string cvtToUTF8(std::wstring_view str) {
  std::string utf8;
  utf8.reserve(str.size() * 3);

  for (char16_t c: str) {
    uint32_t codepoint = static_cast<uint32_t>(c);

    if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
      utf8.push_back('?');
      continue;
    }

    if (codepoint <= 0x7F) { // ASCII
      utf8.push_back(static_cast<char>(codepoint));
    } else if (codepoint <= 0x7FF) { // 2-byte sequence
      utf8.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
      utf8.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else { // 3-byte sequence (U+0800 to U+FFFF)
      utf8.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
      utf8.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
      utf8.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }
  }

  return utf8;
}
} // namespace Helper
