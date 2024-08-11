#pragma once

#include <cstdint>
#include <exception>
#include <format>
#include <map>
#include <memory>
#include <string>
#include <vector>

enum class ConfigType : uint32_t { BOOL, STRING, INT, UINT };

// Size constants
constexpr size_t CONFIG_MAX_STRING_SIZE = 32;

// Config item flags
typedef uint8_t ConfigFlags; // This type should be increased if flags count > 8

constexpr ConfigFlags CONFIG_ITEM_FLAG_CHANGED = 1 << 0;
constexpr ConfigFlags CONFIG_ITEM_FLAG_LOADED  = 1 << 1;

// Config exceptions
class ConfigUnknownEntryException: public std::exception {
  public:
  ConfigUnknownEntryException(std::string_view name) { m_what = std::format("Unknown config key: {}", name); }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

class ConfigInvalidTypeException: public std::exception {
  public:
  ConfigInvalidTypeException(ConfigType ex, ConfigType rc) {
    m_what = std::format("Invalid config entry type (ex: {}, rc: {})", static_cast<uint32_t>(ex), static_cast<uint32_t>(rc));
  }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

class ConfigItem {
  private:
  ConfigType  m_type;
  ConfigFlags m_flags;

  union Value {
    char     str[CONFIG_MAX_STRING_SIZE];
    bool     bvalue;
    uint32_t u32;
    int32_t  i32;

    operator const char*() const { return str; }

    operator bool() const { return bvalue; }

    operator uint32_t() const { return u32; }

    operator int32_t() const { return i32; }
  } m_value;

  public:
  virtual ~ConfigItem() = default;

  ConfigItem(bool value): m_type(ConfigType::BOOL), m_value({.bvalue = value}) {};

  ConfigItem(uint32_t value): m_type(ConfigType::UINT), m_value({.u32 = value}) {};

  ConfigItem(int32_t value): m_type(ConfigType::INT), m_value({.i32 = value}) {}

  ConfigItem(const char* value): m_type(ConfigType::STRING) {
    auto end         = std::string_view(value).copy(m_value.str, sizeof(m_value.str));
    m_value.str[end] = '\0';
  }

  ConfigType getType() const { return m_type; }

  template <typename T>
  T getValue() const {
    // todo safety check?
    return (T)m_value;
  }

  void typeAssert(ConfigType type) {
    if (m_type != type) throw ConfigInvalidTypeException(m_type, type);
  }

  void setValue(bool value) {
    typeAssert(ConfigType::BOOL);
    if (m_value.bvalue == value) return;
    m_value.bvalue = value;
    m_flags |= CONFIG_ITEM_FLAG_CHANGED;
  }

  void setValue(const char* value) {
    typeAssert(ConfigType::STRING);
    auto src = std::string_view(value);
    if (std::string_view(m_value.str).compare(src) == 0) return;
    auto end         = std::string_view(value).copy(m_value.str, sizeof(m_value.str));
    m_value.str[end] = '\0';
    m_flags |= CONFIG_ITEM_FLAG_CHANGED;
  }

  void setValue(uint32_t value) {
    typeAssert(ConfigType::UINT);
    if (m_value.u32 == value) return;
    m_value.u32 = value;
    m_flags |= CONFIG_ITEM_FLAG_CHANGED;
  }

  void setValue(int32_t value) {
    typeAssert(ConfigType::INT);
    if (m_value.i32 == value) return;
    m_value.i32 = value;
    m_flags |= CONFIG_ITEM_FLAG_CHANGED;
  }

  void markAsLoaded() { m_flags |= CONFIG_ITEM_FLAG_LOADED; }

  bool isLoaded() const { return m_flags & CONFIG_ITEM_FLAG_LOADED; }

  bool isChanged() const { return m_flags & CONFIG_ITEM_FLAG_CHANGED; }

  void clearChanged() { m_flags &= ~CONFIG_ITEM_FLAG_CHANGED; }
};

typedef std::map<std::string, ConfigItem> ConfigItemsList;

class IConfig {
  public:
  IConfig()          = default;
  virtual ~IConfig() = default;

  virtual void loadData()                   = 0;
  virtual void saveData(bool force = false) = 0;
  virtual bool isChanged() const            = 0;

  virtual ConfigItem& getItem(std::string_view name) = 0;
};

IConfig& accessConfig();
