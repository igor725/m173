#define _GNU_SOURCE
#include "platform.h"

#include <pthread.h>
#include <signal.h>

namespace {
void (*g_ctrlHandler)(void) = nullptr;
}

namespace Platform {
void SetCurrentThreadName(std::string_view name) {
  pthread_setname_np(pthread_self(), name.data());
}

void RegisterCtrlCHandler(void (*func)(void)) {
  g_ctrlHandler = func;

  signal(SIGINT, [](int) -> void { g_ctrlHandler(); });
}
}; // namespace Platform
