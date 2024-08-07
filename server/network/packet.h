#pragma once

#include "helper.h"
#include "safesock.h"

#include <cstdint>
#include <exception>
#include <sockpp/tcp_socket.h>
#include <string>
#include <vector>

template <typename T>
T bswap(T val) {
  T     retVal;
  char* pVal    = (char*)&val;
  char* pRetVal = (char*)&retVal;
  int   size    = sizeof(T);
  for (int i = 0; i < size; i++) {
    pRetVal[size - 1 - i] = pVal[i];
  }

  return retVal;
}

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

  PacketReader(SafeSocket& sock): m_sock(sock) {}

  template <typename T>
  T readInteger() {
    T rint;
    if (m_sock.read(&rint, sizeof(T))) {
      if (sizeof(T) == 1) return rint;
      return bswap(rint);
    }

    throw ReadException();
  }

  bool readBoolean() { return readInteger<int8_t>() == 1; }

  template <typename T>
  T readFloating() {
    T rfloat;
    if (m_sock.read(&rfloat, sizeof(T))) {
      return bswap(rfloat);
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
        case 2:
        case 4: {
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
  SafeSocket& m_sock;
};

#pragma endregion()

class PacketWriter {
  public:
  PacketWriter(PacketId id) { writeInteger<int8_t>(id); }

  template <typename T>
  void writeInteger(T wint) {
    if (sizeof(T) == 1) {
      m_data.push_back(wint);
      return;
    }

    wint = bswap(wint);
    m_data.insert(m_data.end(), (char*)&wint, (char*)(&(&wint)[1]));
  }

  void writeBoolean(bool wbool) { writeInteger<int8_t>(wbool ? 1 : 0); }

  template <typename T>
  void writeFloating(T wfloat) {
    wfloat = bswap(wfloat);
    m_data.insert(m_data.end(), (char*)&wfloat, (char*)(&(&wfloat)[1]));
  }

  template <typename T>
  void writeString(T& str) {
    writeInteger(static_cast<int16_t>(str.size()));

    if (sizeof(typename T::value_type) < 4) { // Fix some Linux fuckery
      for (auto it = str.begin(); it != str.end(); ++it) {
        writeInteger(static_cast<T::value_type>(*it));
      }
    } else {
      for (auto it = str.begin(); it != str.end(); ++it) {
        writeInteger(static_cast<int16_t>(*it));
      }
    }
  }

  bool sendTo(SafeSocket& sock) { return sock.write(m_data.data(), m_data.size()); }

  private:
  std::vector<char> m_data;
};
