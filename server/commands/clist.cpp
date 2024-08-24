#include "entity/manager.h"
#include "entity/objects/thunderbolt.h"
#include "handler.h"
#include "items/item.h"
#include "runmanager/runmanager.h"
#include "uiwindow/list/workbench.h"
#include "world/world.h"

#include <spdlog/spdlog.h>
#include <sstream>

class Help: public Command {
  public:
  Help(): Command(L"help", L"Shows this message") {}

  bool execute(IPlayer*, std::vector<std::wstring_view>& args, std::wstring& out) final {
    int32_t page = 0;

    if (args.size() > 0) {
      std::wstringstream ss;
      ss << args[0];
      ss >> page;
      --page;
    }

    accessCommandHandler().genHelp(page, 10, out);
    return true;
  }
};

class Stop: public Command {
  public:
  Stop(): Command(L"stop", L"Stops the server") {}

  bool execute(IPlayer*, std::vector<std::wstring_view>&, std::wstring& out) final {
    RunManager::stop();
    return true;
  }
};

class Killme: public Command {
  public:
  Killme(): Command(L"killme", L"Kills you", true) {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>&, std::wstring&) final {
    caller->setHealth(0);
    return true;
  }
};

class Hurtme: public Command {
  public:
  Hurtme(): Command(L"hurtme", L"Hurts you", true) {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
    std::wstringstream ss;
    ss << args[0].data();
    int16_t hcnt;
    ss >> hcnt;

    caller->setHealth(caller->getHealth() - hcnt);
    out = std::format(L"Your current health level: {}", caller->getHealth());
    return true;
  }
};

class Hat: public Command {
  public:
  Hat(): Command(L"hat", L"Set held item as hat", true) {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
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
  Give(): Command(L"give", L"Give some item", true) {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
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
        out = L"\u00a7cFailed, no free space in your inventory";
      }
    } else {
      out = L"\u00a7cFailed, invalid item id";
    }

    return true;
  }
};

class WInfo: public Command {
  public:
  WInfo(): Command(L"winfo", L"Information abot the loaded world", true) {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
    auto& world = accessWorld();

    out = std::format(L"Current world time: {}\nLoaded chunks: {}", world.getTime(), world.getChunksCount());
    return true;
  }
};

class Craft: public Command {
  public:
  Craft(): Command(L"craft", L"Opens the crafting window", true) {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>&, std::wstring&) final {
    caller->createWindow(std::make_unique<WorkbenchWindow>(&caller->getStorage()));
    return true;
  }
};

class Thor: public Command {
  public:
  Thor(): Command(L"thor", L"Summons some lightning strike around", true) {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>&, std::wstring&) final {
    accessEntityManager().AddEntity(createThunderbolt(caller->getPosition()));
    return true;
  }
};

class TP: public Command {
  public:
  TP(): Command(L"tp", L"Teleports you to specified coordinates", true) {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
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

static Help   help_reg;
static Stop   stop_reg;
static Killme killme_reg;
static Hurtme hurtme_reg;
static Hat    hat_reg;
static Give   give_reg;
static WInfo  winfo_reg;
static Craft  craft_reg;
static Thor   thor_reg;
static TP     tp_reg;
