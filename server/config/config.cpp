#include "config.h"

#include "cfgexcept.h"

#include <fstream>
#include <sstream>

namespace {
constexpr std::string_view CONFIG_NAME = "system.cfg";
}

void ConfigItem::setValue(bool value) {
  typeAssert(ConfigType::BOOL);
  if (m_value.bvalue == value) return;
  m_value.bvalue = value;
  m_flags |= CONFIG_ITEM_FLAG_CHANGED;
}

void ConfigItem::setValue(const char* value) {
  typeAssert(ConfigType::STRING);
  auto src = std::string_view(value);
  if (std::string_view(m_value.str).compare(src) == 0) return;
  auto end         = std::string_view(value).copy(m_value.str, sizeof(m_value.str));
  m_value.str[end] = '\0';
  m_flags |= CONFIG_ITEM_FLAG_CHANGED;
}

void ConfigItem::setValue(uint32_t value) {
  typeAssert(ConfigType::UINT);
  if (m_value.u32 == value) return;
  m_value.u32 = std::min(m_limits.umax, std::max(value, m_limits.umin));
  m_flags |= CONFIG_ITEM_FLAG_CHANGED;
}

void ConfigItem::setValue(int32_t value) {
  typeAssert(ConfigType::INT);
  if (m_value.i32 == value) return;
  m_value.i32 = std::min(m_limits.imax, std::max(value, m_limits.imin));
  m_flags |= CONFIG_ITEM_FLAG_CHANGED;
}

void ConfigItem::typeAssert(ConfigType type) {
  if (m_type != type) throw ConfigInvalidTypeException(m_type, type);
}

class Config: public IConfig {
  public:
  Config(ConfigItemsList items): m_list(items) { loadData(); }

  ~Config() { saveData(); }

  void loadData() final {
    std::string tempstr;

    std::ifstream cfile(CONFIG_NAME.data(), std::ios::in);
    if (!cfile.is_open()) {
      saveData(true);
      return;
    }

    while (!cfile.eof()) {
      std::getline(cfile, tempstr, '=');
      if (tempstr.length() == 0) continue;

      try {
        auto& item = getItem(tempstr);
        std::getline(cfile, tempstr); // Receiving the value

        switch (item.getType()) {
          case ConfigType::BOOL: {
            item.setValue(tempstr == "True");
          } break;

          case ConfigType::STRING: {
            item.setValue(tempstr.c_str());
          } break;

          case ConfigType::INT: {
            int32_t tempint;
            std::stringstream(tempstr) >> tempint;
            item.setValue(tempint);
          } break;

          case ConfigType::UINT: {
            uint32_t tempuint;
            std::stringstream(tempstr) >> tempuint;
            item.setValue(tempuint);
          } break;

          default: {
            throw ConfigInvalidTypeException();
          } break;
        }

        // No need to keep "changed" flag for loaded entry
        item.clearChanged();
        item.markAsLoaded();
      } catch (ConfigUnknownEntryException& ex) {
        // This exception can be safely ignored
      }
    }

    for (auto it = m_list.begin(); it != m_list.end(); ++it) {
      if (!it->second.isLoaded()) {
        saveData(true);
        return;
      }
    }
  }

  void saveData(bool force = false) final {
    if (!force && !isChanged()) return;

    std::ofstream cfile(CONFIG_NAME.data(), std::ios::out);

    for (auto it = m_list.begin(); it != m_list.end(); ++it) {
      cfile << it->first << "=";

      auto& item = it->second;
      item.clearChanged();

      switch (item.getType()) {
        case ConfigType::STRING: cfile << item.getValue<const char*>(); break;
        case ConfigType::BOOL: cfile << (item.getValue<bool>() ? "True" : "False"); break;
        case ConfigType::UINT: cfile << item.getValue<uint32_t>(); break;
        case ConfigType::INT: cfile << item.getValue<int32_t>(); break;

        default: {
          throw ConfigInvalidTypeException();
        } break;
      }

      cfile << std::endl;
    }
  }

  bool isChanged() const final {
    for (auto it = m_list.begin(); it != m_list.end(); ++it) {
      if (it->second.isChanged()) return true;
    }

    return false;
  }

  ConfigItem& getItem(std::string_view name) final;

  private:
  ConfigItemsList m_list;
  bool            m_changed;
};

ConfigItem& Config::getItem(std::string_view name) {
  for (auto it = m_list.begin(); it != m_list.end(); ++it) {
    if (it->first == name) return it->second;
  }

  throw ConfigUnknownEntryException(name);
}

IConfig& accessConfig() {
  static Config inst(ConfigItemsList {
      {"logging.level", {"info"}},
      {"bind.max_clients", {40u, 1u, 512u}},
      {"bind.port", {25565u, 0u, 65535u}},
      {"bind.queue_size", {4u, 1u, 128u}},
      {"chunk.load_distance", {10u, 1u, 80u}},
      {"chunk.unload_interval", {300u, 30u}},
      {"world.save_interval", {300u, 30u}},
      {"perms.password", {"changeme"}},
      {"perms.local_op", {true}},
  });
  return inst;
}
