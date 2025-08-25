#include "mcregion.h"

#include "regionfile.h"

#include <format>
#include <spdlog/spdlog.h>
#include <unordered_map>

class MCRegion: public IMCRegion {
  template <typename T>
  using vec2_map = std::unordered_map<IntVector2, std::unique_ptr<T>, IntVector2::HashFunction>;

  static inline IntVector2 packRegionPos(const IntVector2& cpos) { return {cpos.x >> 5, cpos.z >> 5}; }

  public:
  bool loadChunk(const IntVector2& pos, const ChunkUnique& chunk) final {
    spdlog::trace("MCRegion->loadChunk(Vec2({},{}), {})", pos.x, pos.z, (void*)&chunk);
    auto& rfile = getFile(packRegionPos(pos));
    return rfile.readChunk(pos, chunk);
  }

  bool saveChunk(const IntVector2& pos, const ChunkUnique& chunk) final {
    spdlog::trace("MCRegion->saveChunk(Vec2({},{}), {})", pos.x, pos.z, (void*)&chunk);
    auto& rfile = getFile(packRegionPos(pos));
    return rfile.writeChunk(pos, chunk);
  }

  IRegionFile& getFile(const IntVector2& pos) {
    auto it = m_files.find(pos);
    if (it == m_files.end()) {
      spdlog::trace("No mcr0 file found in file cache ({},{})", pos.x, pos.z);
      for (it = m_files.begin(); it != m_files.end();) {
        if ((*it->second).canBeUnloaded()) {
          it = m_files.erase(it);
          continue;
        }

        ++it;
      }

      auto   rfile = createRegionFile(std::format("regions/r.{}.{}.mcr0", pos.x, pos.z));
      auto&& f     = m_files.emplace(std::make_pair(pos, std::move(rfile)));

      return *f.first->second;
    }

    spdlog::trace("RegionFile cache hit ({},{})", pos.x, pos.z);
    return *it->second;
  }

  private:
  vec2_map<IRegionFile> m_files = {};
};

IMCRegion& accessRegionManager() {
  static MCRegion inst;
  return inst;
}
