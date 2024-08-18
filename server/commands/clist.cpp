#include "handler.h"
#include "items/item.h"
#include "runmanager/runmanager.h"

#include <spdlog/spdlog.h>
#include <sstream>

class Stop: public Command {
  public:
  Stop(): Command(L"stop", L"Stops the server") {}

  bool execute(IPlayer*, std::vector<std::wstring_view>&, std::wstring& out) final {
    out = L"Stopping the server...";
    RunManager::stop();
    return true;
  }
};

class Killme: public Command {
  public:
  Killme(): Command(L"killme", L"Kills you", true) {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
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
      out = L"Usage /give <item id> [item count]";
      return true;
    }
    std::wstringstream ss;
    ss << args[0];
    ItemId  iid;
    int16_t count  = 64;
    int16_t damage = 0;
    ss >> iid;

    if (auto item = Item::getById(iid)) {
      if (args.size() > 1) {
        ss.clear();
        ss << args[1];
        ss >> count;
      }

      if (args.size() > 2) {
        ss.clear();
        ss << args[2];
        ss >> damage;
      }

      count = std::min(item->getStackLimit(), count);

      // Container should be used there, since we operate with absolute SlotIDs (current selected hotbar item)
      auto& cont = caller->getInventoryContainer();

      SlotId slot;
      if (cont.push(ItemStack(iid, count, damage), &slot, caller->getHeldItemSlotId())) {
        caller->resendItem(cont.getItem(slot));
        out = std::format(L"Given {} of {}:{}", count, iid, damage);
      } else {
        out = L"\u00a7cFailed, no free space in your inventory";
      }
    } else {
      out = L"\u00a7cFailed, invalid item id";
    }

    return true;
  }
};

static Stop   stop_reg;
static Killme killme_reg;
static Hurtme hurtme_reg;
static Hat    hat_reg;
static Give   give_reg;
