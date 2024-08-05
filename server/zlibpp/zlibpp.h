#pragma once

#include <exception>
#include <format>
#include <fstream>
#include <string>

class IZLibPP {
  public:
  IZLibPP()          = default;
  virtual ~IZLibPP() = default;

  virtual void setInput(const void* data, size_t size) = 0;
  virtual void setOutput(void* data, size_t size)      = 0;

  virtual bool tick() = 0;

  virtual unsigned long getAvailableOutput() const = 0;
  virtual unsigned long getTotalOutput() const     = 0;
  virtual unsigned long getAvailableInput() const  = 0;
};

std::unique_ptr<IZLibPP> createDecompressor();
std::unique_ptr<IZLibPP> createCompressor();
