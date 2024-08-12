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

  void reset() final {
    m_state     = Idle;
    m_lastTotal = 0;
    setInput(nullptr, 0);
    setOutput(nullptr, 0);
    inflateReset(&m_stream);
  }

  void setInput(const void* data, size_t size) final {
    m_stream.avail_in = size;
    m_stream.next_in  = (Bytef*)data;
  }

  void setOutput(void* data, size_t size) final {
    m_stream.avail_out = size;
    m_stream.next_out  = (Bytef*)data;
  }

  State tick() final {
    if (m_state == Idle) m_state = InProcess;

    int ret;
    m_lastTotal = m_stream.total_out;
    switch (ret = inflate(&m_stream, m_stream.avail_in == 0 ? Z_FINISH : Z_NO_FLUSH)) {
      case Z_OK: {
        if (m_stream.avail_out == 0) return MoreOut;
      }

      default: throw ZlibException(ret);
    }

    return m_state;
  }

  unsigned long getAvailableOutput() const final { return m_stream.avail_out; }

  unsigned long getAvailableInput() const final { return m_stream.avail_in; }

  unsigned long getTotalOutput() const final { return m_stream.total_out; }

  unsigned long getFrameSize() const final { return m_stream.total_out - m_lastTotal; }

  private:
  z_stream m_stream    = {0};
  uLong    m_lastTotal = 0;
  State    m_state     = Idle;
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

  void reset() final {
    m_state     = Idle;
    m_lastTotal = 0;
    setInput(nullptr, 0);
    setOutput(nullptr, 0);
    deflateReset(&m_stream);
  }

  void setInput(const void* data, size_t size) final {
    m_stream.avail_in = size;
    m_stream.next_in  = (Bytef*)data;
  }

  void setOutput(void* data, size_t size) final {
    m_stream.avail_out = size;
    m_stream.next_out  = (Bytef*)data;
  }

  State tick() final {
    if (m_state == Done) return m_state;

    switch (m_state) {
      case Idle:
      case MoreOut: {
        m_state = InProcess;
      } break;
    }

    if (m_stream.avail_in == 0 && m_state == InProcess) m_state = Finishing;

    int ret;
    m_lastTotal = m_stream.total_out;
    switch (ret = deflate(&m_stream, m_stream.avail_in == 0 ? Z_FINISH : Z_NO_FLUSH)) {
      case Z_OK: {
        if (m_stream.avail_out == 0) return MoreOut;
      } break;
      case Z_STREAM_END: {
        if (m_state == Finishing) return m_state = Done;
        throw ZlibException(ret);
      } break;
      case Z_STREAM_ERROR: {
        if (m_stream.avail_out == 0) return MoreOut;
      } break;

      default: throw ZlibException(ret);
    }

    return m_state;
  }

  unsigned long getAvailableOutput() const final { return m_stream.avail_out; }

  unsigned long getAvailableInput() const final { return m_stream.avail_in; }

  unsigned long getTotalOutput() const final { return m_stream.total_out; }

  unsigned long getFrameSize() const final { return m_stream.total_out - m_lastTotal; }

  private:
  z_stream m_stream    = {0};
  uLong    m_lastTotal = 0;
  State    m_state     = Idle;
};

std::unique_ptr<IZLibPP> createCompressor() {
  return std::make_unique<Compressor>();
}

std::unique_ptr<IZLibPP> createDecompressor() {
  return std::make_unique<Decompressor>();
}
