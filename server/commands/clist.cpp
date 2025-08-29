#include "config/config.h"
#include "entity/manager.h"
#include "entity/objects/thunderbolt.h"
#include "handler.h"
#include "items/item.h"
#include "playerui/windows/list/workbench.h"
#include "runmanager/runmanager.h"
#include "script/script.h"
#include "world/world.h"

#include <format>
#include <spdlog/spdlog.h>
#include <sstream>

class Help: public Command {
  public:
  Help(): Command(L"help", L"Show this message") {}

  bool execute(Entities::PlayerBase* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
    int32_t page = 0;

    if (args.size() > 0) {
      std::wstringstream ss;
      ss << args[0];
      ss >> page;
      --page;
    }

    accessCommandHandler().genHelp(page, 10, caller, out);
    return true;
  }
};

class Stop: public Command {
  public:
  Stop(): Command(L"stop", L"Stop the server", OperatorOnly) {}

  bool execute(Entities::PlayerBase*, std::vector<std::wstring_view>&, std::wstring& out) final {
    RunManager::stop();
    return true;
  }
};

class Hat: public Command {
  public:
  Hat(): Command(L"hat", L"Set held item as hat", PlayerOnly) {}

  bool execute(Entities::PlayerBase* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
    auto& heldItem = caller->getHeldItem();
    auto& storage  = caller->getStorage();
    if (heldItem.validate()) {
      auto& headSlot = storage.getByOffset(storage.getArmorOffset());

      headSlot = heldItem.splitStack(1);
      caller->resendItem(heldItem);
      caller->resendItem(headSlot);
    }
    return true;
  }
};

class Give: public Command {
  public:
  Give(): Command(L"give", L"Give some item", OperatorOnly | PlayerOnly) {}

  bool execute(Entities::PlayerBase* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
    if (args.size() == 0) {
      out = L"Usage /give <item id>:[item damage] [item count]";
      return true;
    }

    ItemStack is;

    std::wstringstream ss;

    ss << args[0];
    ss >> is.itemId;
    if (auto item = Item::getById(is.itemId)) {
      is.stackSize = item->getStackLimit();

      if (ss.peek() == L':') {
        wchar_t t;
        ss.read(&t, 1);
        ss >> is.itemDamage;
      }

      if (args.size() > 1) {
        ss.clear();
        ss << args[1];
        ss >> is.stackSize;
        is.stackSize = std::min(item->getStackLimit(), is.stackSize);
      }

      // Container should be used there, since we operate with absolute SlotIDs (current selected hotbar item)
      auto& cont = caller->getInventoryContainer();

      SlotId slot;
      if (cont.push(is, &slot, caller->getHeldItemSlotId())) {
        caller->resendItem(cont.getItem(slot));
        out = std::format(L"\u00a7eGiven {} of {}:{}", is.stackSize, is.itemId, is.itemDamage);
      } else {
        out = L"No free space in your inventory";
        return false;
      }
    } else {
      out = L"Invalid item id";
      return false;
    }

    return true;
  }
};

class WInfo: public Command {
  public:
  WInfo(): Command(L"winfo", L"Information abot the loaded world") {}

  bool execute(Entities::PlayerBase* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
    auto& world = accessWorld();

    out = std::format(L"Current world time: {}\nLoaded chunks: {}", world.getTime(), world.getChunksCount());
    return true;
  }
};

class Craft: public Command {
  public:
  Craft(): Command(L"craft", L"Open the crafting window", PlayerOnly) {}

  bool execute(Entities::PlayerBase* caller, std::vector<std::wstring_view>&, std::wstring&) final {
    caller->createWindow(std::make_unique<WorkbenchWindow>(&caller->getStorage()));
    return true;
  }
};

class Thor: public Command {
  public:
  Thor(): Command(L"thor", L"Summon some lightning strike around", OperatorOnly | PlayerOnly) {}

  bool execute(Entities::PlayerBase* caller, std::vector<std::wstring_view>&, std::wstring&) final {
    Entities::Access::manager().AddEntity(Entities::Create::thunderbolt(caller->getPosition()));
    return true;
  }
};

class TP: public Command {
  public:
  TP(): Command(L"tp", L"Teleport to specified coordinates", OperatorOnly) {}

  bool execute(Entities::PlayerBase* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
    if (args.size() < 3) {
      out = L"Usage: /tp <x> <y> <z>";
      return true;
    }

    std::wstringstream ss;
    DoubleVector3      npos;

    ss << args[0];
    ss >> npos.x;
    ss.clear();
    ss << args[1];
    ss >> npos.y;
    ss.clear();
    ss << args[2];
    ss >> npos.z;

    caller->teleportPlayer(npos);
    return true;
  }
};

class Lua: public Command {
  public:
  Lua(): Command(L"lua", L"Manage Lua VM", OperatorOnly) {}

  bool execute(Entities::PlayerBase* executor, std::vector<std::wstring_view>& args, std::wstring& out) {
    if (args.size() < 1) {
    usage:
      out = L"Usage: /lua <reload/reloadall/status/load> [script file name]";
      return true;
    }

    if (args[0] == L"reload") {
      out = accessScript().reload(args[1]) ? L"\u00a7aScript reloaded successfully" : L"\u00a7cScript not found!";
    } else if (args[0] == L"reloadall") {
      accessScript().reloadAll();
      out = L"All scripts reloaded!";
    } else if (args[0] == L"status") {
      if (args.size() > 1) {
        accessScript().getStatus(out, args[1]);
      } else {
        accessScript().getStatus(out);
      }
    } else if (args[0] == L"load") {
      if (args.size() > 1) {
        if (accessScript().openScript(args[1])) {
          out = L"\u00a7aScript loaded successfully";
        } else {
          out = L"Failed, check console for more info";
          return false;
        }
      } else {
        out = L"Usage: /lua load <script name>";
      }
    } else {
      goto usage;
    }

    return true;
  }
};

class Pwd: public Command {
  public:
  Pwd(): Command(L"pwd", L"Authorize as server's operator", PlayerOnly) {}

  bool execute(Entities::PlayerBase* executor, std::vector<std::wstring_view>& args, std::wstring& out) {
    if (args.size() < 1) {
      out = L"Usage: /pwd <password>";
      return true;
    }

    if (executor->isOperator()) {
      out = L"\u00a7aYou're already an operator!";
      return true;
    }

    auto pwd = std::string_view(accessConfig().getItem("perms.password").getValue<const char*>());
    if (Helper::strcmp(pwd, args[0])) {
      out = L"\u00a7aYou're operator now!";
      executor->setOperator(true);
    } else {
      out = L"Incorrect password!";
      return false;
    }

    return true;
  }
};

void Command::regAll() {
  static Help  help_reg;
  static Stop  stop_reg;
  static Hat   hat_reg;
  static Give  give_reg;
  static WInfo winfo_reg;
  static Craft craft_reg;
  static Thor  thor_reg;
  static TP    tp_reg;
  static Lua   lua_reg;
  static Pwd   pwd_reg;
}
