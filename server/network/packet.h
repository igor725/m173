#pragma once

#include "helper.h"
#include "items/itemstack.h"

#include <cstdint>
#include <exception>
#include <string>
#include <vector>

namespace {
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
} // namespace

#ifdef M173_ACTIVATE_READER_API
#pragma region(Reader)

#include "safesock.h"

class PacketReader {
  public:
#pragma region(Exceptions)

  class ReadException: public std::exception {
public:
    ReadException() {}

    const char* what() const noexcept override { return "Failed to read packet"; }

private:
  };

#pragma endregion()

  PacketReader(SafeSocket& sock): m_sock(sock) {}

  protected:
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
#endif

#pragma region(Writer)

class MetaDataStream;

class PacketWriter {
  public:
  friend class MetaDataStream;

  PacketWriter(PacketId id);

  PacketWriter(PacketId id, int32_t psize);

  template <typename T>
  bool sendTo(T& sock) {
    return sock.write(m_data.data(), m_data.size());
  }

  protected:
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
  void writeString(const T& str) {
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

  void writeMotion(const DoubleVector3& motion) {
    writeInteger<int16_t>(static_cast<int16_t>(motion.x * 8000.0));
    writeInteger<int16_t>(static_cast<int16_t>(motion.y * 8000.0));
    writeInteger<int16_t>(static_cast<int16_t>(motion.z * 8000.0));
  }

  void writeIVector(const IntVector3& vec) {
    writeInteger<int32_t>(vec.x);
    writeInteger<int32_t>(vec.y);
    writeInteger<int32_t>(vec.z);
  }

  void writeAIVector(const DoubleVector3& pos) {
    writeInteger(floor_double(pos.x * 32.0));
    writeInteger(floor_double(pos.y * 32.0));
    writeInteger(floor_double(pos.z * 32.0));
  }

  void writeABVector(const DoubleVector3& pos) {
    writeInteger<int8_t>(floor_double8(pos.x * 32.0));
    writeInteger<int8_t>(floor_double8(pos.y * 32.0));
    writeInteger<int8_t>(floor_double8(pos.z * 32.0));
  }

  private:
  inline int32_t floor_double(double_t d) { return int32_t(std::round(d)); }

  inline int8_t floor_double8(double_t d) { return int8_t(std::round(d)); }

  protected:
  std::vector<char> m_data;
};

class MetaDataStream {
  public:
  MetaDataStream(PacketWriter& pw): m_writer(pw) {}

  ~MetaDataStream() {
    // Finishing the stream with the terminator byte
    m_writer.writeInteger<int8_t>(0x7f);
  }

  void putByte(int valueid, int8_t value) {

    putHeader(0, valueid);
    m_writer.writeInteger<int8_t>(value);
  }

  void putShort(int valueid, int16_t value) {
    putHeader(1, valueid);
    m_writer.writeInteger<int16_t>(value);
  }

  void putInt(int valueid, int32_t value) {
    putHeader(2, valueid);
    m_writer.writeInteger<int32_t>(value);
  }

  void putFloat(int valueid, float_t value) {
    putHeader(3, valueid);
    m_writer.writeFloating<float_t>(value);
  }

  void putString(int valueid, const std::wstring& str) {
    putHeader(4, valueid);
    m_writer.writeString(str);
  }

  void putItem(int valueid, const ItemStack& is) {
    putHeader(5, valueid);
    m_writer.writeInteger<ItemId>(is.itemId);
    m_writer.writeInteger<int16_t>(is.stackSize);
    m_writer.writeInteger<int16_t>(is.itemDamage);
  }

  void putVector(int valueid, const IntVector3& vec) {
    putHeader(5, valueid);
    m_writer.writeIVector(vec);
  }

  private:
  void putHeader(int8_t type, int8_t valueid) { m_writer.writeInteger<int8_t>((type << 5 | (valueid & 31)) & 255); }

  PacketWriter& m_writer;
};

#pragma endregion()
