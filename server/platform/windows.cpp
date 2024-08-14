#include "platform.h"

#include <Windows.h>

namespace {
void (*g_ctrlHandler)(void) = nullptr;
}

namespace Platform {
void SetCurrentThreadName(std::string_view name) {
  SetThreadDescription(GetCurrentThread(), std::wstring(name.begin(), name.end()).c_str());
}

void RegisterCtrlCHandler(void (*func)(void)) {
  g_ctrlHandler = func;
  SetConsoleCtrlHandler(
      [](DWORD dwCtrlType) -> BOOL {
        switch (dwCtrlType) {
          case CTRL_C_EVENT: {
            g_ctrlHandler();
            return true;
          } break;

          default: return false;
        }
      },
      true);
}
}; // namespace Platform
