#include "glist.h"

#include "list/flat.h"

std::unique_ptr<IGenerator> createFlat(int64_t seed) {
  return std::make_unique<FlatGen>(seed);
}
