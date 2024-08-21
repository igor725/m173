#pragma once

#include "interface.h"

#include <memory>

std::unique_ptr<IGenerator> createFlat(int64_t seed);
