#pragma once

#include <cstddef>
#include <memory>

class IZLibPP {
  public:
  enum State {
    Idle,
    InProcess,
    MoreOut,
    Finishing,
    Fatal,
    Done,
  };

  IZLibPP()          = default;
  virtual ~IZLibPP() = default;

  virtual void setInput(const void* data, size_t size) = 0;
  virtual void setOutput(void* data, size_t size)      = 0;

  virtual State tick()  = 0;
  virtual void  reset() = 0;

  virtual unsigned long getAvailableOutput() const = 0;
  virtual unsigned long getTotalOutput() const     = 0;
  virtual unsigned long getAvailableInput() const  = 0;
  virtual unsigned long getFrameSize() const       = 0;
};

std::unique_ptr<IZLibPP> createDecompressor();
std::unique_ptr<IZLibPP> createCompressor();
