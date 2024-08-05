#include "zlibpp.h"

#include "zlibexc.h"

#include <memory>
#include <zlib.h>

class Decompressor: public IZLibPP {
  public:
  Decompressor() {
    int ret;
    if ((ret = inflateInit(&m_stream)) != Z_OK) {
      throw ZlibException(ret);
    }
  }

  ~Decompressor() { inflateEnd(&m_stream); }

  void setInput(const void* data, size_t size) final {
    m_stream.avail_in = size;
    m_stream.next_in  = (Bytef*)data;
  }

  void setOutput(void* data, size_t size) final {
    m_stream.avail_out = size;
    m_stream.next_out  = (Bytef*)data;
  }

  bool tick() final {
    int ret;
    switch (ret = inflate(&m_stream, m_stream.avail_in == 0 ? Z_FINISH : Z_NO_FLUSH)) {
      case Z_STREAM_END: return true;
      case Z_OK: return false;

      default: throw ZlibException(ret);
    }
  }

  unsigned long getAvailableOutput() const final { return m_stream.avail_out; }

  unsigned long getAvailableInput() const final { return m_stream.avail_in; }

  unsigned long getTotalOutput() const final { return m_stream.total_out; }

  private:
  z_stream m_stream = {0};
};

class Compressor: public IZLibPP {
  public:
  Compressor() {
    int ret;
    if ((ret = deflateInit(&m_stream, Z_DEFAULT_COMPRESSION)) != Z_OK) {
      throw ZlibException(ret);
    }
  }

  ~Compressor() { deflateEnd(&m_stream); }

  void setInput(const void* data, size_t size) final {
    m_stream.avail_in = size;
    m_stream.next_in  = (Bytef*)data;
  }

  void setOutput(void* data, size_t size) final {
    m_stream.avail_out = size;
    m_stream.next_out  = (Bytef*)data;
  }

  bool tick() final {
    int ret;
    switch (ret = deflate(&m_stream, m_stream.avail_in == 0 ? Z_FINISH : Z_NO_FLUSH)) {
      case Z_STREAM_END: return true;
      case Z_OK: return false;

      default: throw ZlibException(ret);
    }
  }

  unsigned long getAvailableOutput() const final { return m_stream.avail_out; }

  unsigned long getAvailableInput() const final { return m_stream.avail_in; }

  unsigned long getTotalOutput() const final { return m_stream.total_out; }

  private:
  z_stream m_stream = {0};
};

std::unique_ptr<IZLibPP> createCompressor() {
  return std::make_unique<Compressor>();
}

std::unique_ptr<IZLibPP> createDecompressor() {
  return std::make_unique<Decompressor>();
}
