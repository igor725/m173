#include "config.h"

#include <fstream>
#include <sstream>

namespace {
constexpr std::string_view CONFIG_NAME = "system.cfg";
}

class Config: public IConfig {
  public:
  Config(ConfigItemsList items): m_list(items) { loadData(); }

  ~Config() { saveData(); }

  void loadData() final {
    std::string tempstr;

    std::ifstream cfile(CONFIG_NAME.data(), std::ios::in);
    if (!cfile.is_open()) {
      saveData();
      return;
    }

    while (!cfile.eof()) {
      std::getline(cfile, tempstr, '=');
      if (tempstr.length() == 0) continue;

      try {
        auto& item = getItem(tempstr);
        std::getline(cfile, tempstr);

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
        }

        // No need to keep "changed" flag for loaded entry
        item.clearChanged();
      } catch (ConfigUnknownEntryException& ex) {
        // This exception can be safely ignored
      }
    }
  }

  void saveData() final {
    if (!isChanged()) return;

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
      }

      cfile << std::endl;
    }
  }

  bool isChanged() final {
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
      {"bind.port", {25565u}},
      {"bind.queue_size", {4u}},
      {"chunk.load_distance", {20u}},
  });
  return inst;
}
