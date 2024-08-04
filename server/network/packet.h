#pragma once

#include "helper.h"

#include <cstdint>
#include <exception>
#include <sockpp/tcp_socket.h>
#include <string>
#include <vector>

#pragma region(Reader)

class PacketReader {
  public:
#pragma region(Reader::Types)

  enum class Direction {
    Both,
    ToClient,
    ToServer,
  };

#pragma endregion()

#pragma region(Exceptions)

  class ReadException: public std::exception {
public:
    ReadException() {}

    const char* what() const noexcept override { return "Failed to read packet"; }

private:
  };

#pragma endregion()

  PacketReader(sockpp::tcp_socket& sock, Direction dir = Direction::Both): m_sock(sock), m_dir(dir) {}

  template <typename T>
  T readInteger() {
    T rint;
    if (m_sock.read(&rint, sizeof(T)) == sizeof(T)) {
      switch (sizeof(T)) {
        case 1: return rint;
        case 2: return static_cast<T>(_byteswap_ushort(rint));
        case 4: return static_cast<T>(_byteswap_ulong(rint));
        case 8: return static_cast<T>(_byteswap_uint64(rint));
      }
    }

    throw ReadException();
  }

  bool readBoolean() { return readInteger<int8_t>() == 1; }

  template <typename T>
  T readFloating() {
    T rfloat;
    if (m_sock.read(&rfloat, sizeof(T)) == sizeof(T)) {
      return rfloat;
    }

    throw ReadException();
  }

  template <typename T>
  void readString(T& dst) {
    auto strSize = readInteger<int16_t>();
    dst.clear();
    dst.reserve(strSize);

    while (strSize--) {
      typename T::value_type sym;

      switch (sizeof(typename T::value_type)) {
        case 1: {
          sym = readInteger<int8_t>();
        } break;
        case 2: {
          sym = readInteger<int16_t>();
        } break;

        default: throw ReadException();
      }

      dst.push_back(sym);
    }

    if (strSize < 0) return;

    throw ReadException();
  }

  private:
  sockpp::tcp_socket& m_sock;
  Direction           m_dir;
};

#pragma endregion()

class PacketWriter {
  public:
  PacketWriter(PacketId id) { writeInteger<int8_t>(id); }

  template <typename T>
  void writeInteger(T wint) {
    switch (sizeof(T)) {
      case 1: {
        m_data.push_back(wint);
        return;
      } break;
      case 2: {
        wint = static_cast<T>(_byteswap_ushort(wint));
      } break;
      case 4: {
        wint = static_cast<T>(_byteswap_ulong(wint));
      } break;
      case 8: {
        wint = static_cast<T>(_byteswap_uint64(wint));
      } break;
    }

    m_data.insert(m_data.end(), (char*)&wint, (char*)(&(&wint)[1]));
  }

  void writeBoolean(bool wbool) { writeInteger<int8_t>(wbool ? 1 : 0); }

  template <typename T>
  void writeFloating(T wfloat) {
    m_data.insert(m_data.end(), (char*)&wfloat, (char*)(&(&wfloat)[1]));
  }

  template <typename T>
  void writeString(T& str) {
    writeInteger(static_cast<int16_t>(str.size()));

    for (auto it = str.begin(); it != str.end(); ++it) {
      writeInteger(static_cast<T::value_type>(*it));
    }
  }

  void sendTo(sockpp::tcp_socket& sock) { sock.write(m_data.data(), m_data.size()); }

  private:
  std::vector<char> m_data;
};
