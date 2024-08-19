#pragma once

#include "config.h"

#include <exception>
#include <format>
#include <string>

class ConfigUnknownEntryException: public std::exception {
  public:
  ConfigUnknownEntryException(std::string_view name) { m_what = std::format("Unknown config key: {}", name); }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

class ConfigInvalidTypeException: public std::exception {
  public:
  ConfigInvalidTypeException() { m_what = "ConfigItem has UNSPECIFIED type"; }

  ConfigInvalidTypeException(ConfigType ex, ConfigType rc) {
    m_what = std::format("Invalid config entry type (ex: {}, rc: {})", static_cast<uint32_t>(ex), static_cast<uint32_t>(rc));
  }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};
