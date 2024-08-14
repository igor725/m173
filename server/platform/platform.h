#pragma once

#include <string>

namespace Platform {
void SetCurrentThreadName(std::string_view name);
void RegisterCtrlCHandler(void (*func)(void));
}; // namespace Platform
