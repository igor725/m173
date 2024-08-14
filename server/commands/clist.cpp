#include "handler.h"

#include <spdlog/spdlog.h>
#include <sstream>

class Test: public Command {
  public:
  Test(): Command(L"test", L"Test message") {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
    (void)caller;
    (void)args;
    out = L"Yay! Test command!!!";
    return true;
  }
};

class Killme: public Command {
  public:
  Killme(): Command(L"killme", L"Kills you") {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
    caller->setHealth(0);
    return true;
  }
};

class Hurtme: public Command {
  public:
  Hurtme(): Command(L"hurtme", L"Hurts you") {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
    std::wstringstream ss;
    ss << args[0].data();
    int16_t hcnt;
    ss >> hcnt;

    caller->setHealth(caller->getHealth() - hcnt);
    return true;
  }
};

class Give: public Command {
  public:
  Give(): Command(L"give", L"Give some item") {}

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

    SlotId slot;
    if (caller->getContainer().push(ItemStack(iid, count, damage), &slot, caller->getHeldItemSlotId())) {
      caller->resendItem(caller->getContainer().getItem(slot));
      out = std::format(L"Given {} of {}:{}", count, iid, damage);
    } else {
      out = L"\u00a7cFailed, invalid item id or no free space in your inventory";
    }

    return true;
  }
};

static Killme killme_reg;
static Hurtme hurtme_reg;
static Test   test_reg;
static Give   give_reg;
