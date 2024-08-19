#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>

enum class ConfigType : uint32_t { UNSPECIFIED, BOOL, STRING, INT, UINT };

// Size constants
constexpr size_t CONFIG_MAX_STRING_SIZE = 32;

// Config item flags
typedef uint8_t ConfigFlags; // This type should be increased if flags count > 8

constexpr ConfigFlags CONFIG_ITEM_FLAG_CHANGED = 1 << 0;
constexpr ConfigFlags CONFIG_ITEM_FLAG_LOADED  = 1 << 1;

class ConfigItem {
  private:
  ConfigType  m_type  = ConfigType::UNSPECIFIED;
  ConfigFlags m_flags = 0;

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

  union Limits {
    struct {
      uint32_t umin, umax;
    };

    struct {
      int32_t imin, imax;
    };

    Limits(): imin(0), imax(0) {}

    Limits(uint32_t min, uint32_t max): umin(min), umax(max) {
      if (min > max) abort();
    }

    Limits(int32_t min, int32_t max): imin(min), imax(max) {
      if (min > max) abort();
    }
  } m_limits;

  public:
  virtual ~ConfigItem() = default;

  ConfigItem(bool value): m_type(ConfigType::BOOL), m_value({.bvalue = value}) {};

  ConfigItem(uint32_t value, uint32_t min = 0, uint32_t max = UINT32_MAX): m_type(ConfigType::UINT), m_value({.u32 = value}), m_limits(min, max) {};

  ConfigItem(int32_t value, int32_t min = INT32_MIN, int32_t max = INT32_MAX): m_type(ConfigType::INT), m_value({.i32 = value}), m_limits(min, max) {}

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

  void typeAssert(ConfigType type);

  void setValue(bool value);

  void setValue(const char* value);

  void setValue(uint32_t value);

  void setValue(int32_t value);

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
