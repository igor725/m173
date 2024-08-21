#include "regionfile.h"

#include "world/chunkCompression.h"
#include "zlibpp/zlibpp_unique.h"

#include <array>
#include <chrono>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <vector>

namespace {
typedef uint32_t RegOff;

constexpr uint32_t SECTOR_SIZE            = 4096;
constexpr uint32_t OFFSET_TAB_SZ          = (SECTOR_SIZE / sizeof(RegOff));
constexpr uint32_t START_COMP_BUF_SZ      = 512;
constexpr uint32_t MAX_SECTORS_PER_OFFSET = 0xff;

enum CompressionType : uint8_t {
  Unspecified,
  GZipNBT           = 1,
  ZlibNBT           = 2,
  UncompressedNBT   = 3,
  UncompressedChunk = 4,
  Zlib              = 5,
};

class UnsupportedCompression: public std::exception {
  public:
  UnsupportedCompression(CompressionType t) {
    static std::array<const char*, 6> types = {"Unspecified", "GZipNBT", "ZlibNBT", "UncompressedNBT", "UncompressedChunk", "Zlib"};

    m_what = std::format("Unsupported format ({}) passed to RegionFile reader!", types[t < types.size() ? t : 0]);
  }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

#pragma pack(push, 1)

struct PayloadHeader {
  uint32_t        length;
  CompressionType compression;
};

#pragma pack(pop)

static UniqueZlibPP g_compr(createCompressor());
static UniqueZlibPP g_decom(createDecompressor());
} // namespace

class InvalidRegionFileException;

class RegionFile: public IRegionFile {
  std::array<char, SECTOR_SIZE> m_emptySector = {'\0'};

  public:
  RegionFile(const std::string& fname): m_lastAccess(), m_offsets({0}) {
    if (!std::filesystem::exists(fname)) {
      std::filesystem::path fpath(fname);
      fpath.remove_filename();
      std::filesystem::create_directory(fpath);
      std::ofstream(fname).close();
    }

    m_file.exceptions(m_file.exceptions() | std::ios::failbit | std::fstream::badbit);
    m_file.open(fname, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);

    size_t fsize = m_file.tellg();
    m_file.seekg(0, std::ios::beg);

    if (fsize < SECTOR_SIZE) {
      m_file.seekp(0, std::ios::beg);
      m_file.write((char*)m_offsets.data(), m_offsets.size() * sizeof(RegOff));
      fsize = SECTOR_SIZE;
    }

    {
      m_file.seekp(0, std::ios::end);
      char empty = '\0';
      for (; (fsize & (SECTOR_SIZE - 1)) != 0; ++fsize) {
        m_file.write(&empty, 1);
      }

      m_file.flush();
    }

    m_file.seekg(0, std::ios::beg);

    auto snum = fsize / uintptr_t(SECTOR_SIZE);
    m_sectorFree.resize(snum, true);
    m_sectorFree.at(0) = false; // Sector 0x0 is a offsets table
    m_file.read(reinterpret_cast<char*>(m_offsets.data()), m_offsets.size() * sizeof(RegOff));

    uint32_t sec_count = 1;
    for (auto it = m_offsets.begin(); it != m_offsets.end(); ++it) {
      auto offset     = (*it);
      auto secNum     = offset >> 8;
      auto secAlloced = offset & MAX_SECTORS_PER_OFFSET;

      if (offset > 0 && (secNum + secAlloced) <= m_sectorFree.size()) changeFree(secNum, secAlloced, false);
    }

    m_lastAccess = std::chrono::system_clock::now();
  }

  ~RegionFile() {}

  bool canBeUnloaded() final {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now() - m_lastAccess) > seconds(120);
  }

  bool writeChunk(const IntVector2& pos, Chunk& chunk) final {
    m_lastAccess = std::chrono::system_clock::now();

    auto     offset       = m_offsets[getIndex(pos)];
    uint32_t sectorNum    = offset >> 8;
    uint32_t sectorsAlloc = offset & MAX_SECTORS_PER_OFFSET;
    uint32_t comprSize    = compressChunk(chunk);
    uint32_t sectorsNeed  = comprSize / SECTOR_SIZE + 1;

    if (sectorsNeed > MAX_SECTORS_PER_OFFSET) return false;

    if (sectorNum != 0 && sectorsAlloc == sectorsNeed) {
      return writeComprDataToSector(sectorNum, comprSize);
    } else {
      // Free now unused sectors
      changeFree(sectorNum, sectorsAlloc, true);

      uint32_t freeSectorStart = 0;
      uint32_t freeSectorsNum  = 0;

      auto currSect = std::find(m_sectorFree.begin(), m_sectorFree.end(), true);
      for (; currSect != m_sectorFree.end(); ++currSect) {
        if (freeSectorsNum > 0) {
          freeSectorsNum = *currSect ? ++freeSectorsNum : 0;
        } else if (*currSect) {
          freeSectorStart = std::distance(m_sectorFree.begin(), currSect);
          freeSectorsNum  = 1;
        }

        if (freeSectorsNum >= sectorsNeed) break;
      }

      if (freeSectorsNum >= sectorsNeed) {
        sectorNum = freeSectorStart;
        changeFree(freeSectorStart, sectorsNeed, false);
      } else {
        m_file.flush();
        sectorNum = m_sectorFree.size();
        m_sectorFree.resize(sectorNum + sectorsNeed);

        m_file.seekp(0, std::ios::end);
        for (int i = 0; i < sectorsNeed; ++i) {
          m_file.write(m_emptySector.data(), m_emptySector.size());
        }
      }

      if (!writeComprDataToSector(sectorNum, comprSize)) return false;
      setOffset(pos, (sectorNum << 8) | sectorsNeed);
      return true;
    }

    return false;
  }

  bool readChunk(const IntVector2& pos, Chunk& chunk) final {
    m_lastAccess = std::chrono::system_clock::now();
    auto offset  = m_offsets[getIndex(pos)];
    if (offset == 0) return false;
    return readFromSector(offset >> 8, chunk);
  }

  private:
  void changeFree(uint32_t begin, uint32_t length, bool value) {
    auto it = m_sectorFree.begin() + begin;
    std::fill(it, it + length, value);
  }

  int32_t compressChunk(Chunk& chunk) {
    auto compacq = g_compr.acquire();
    auto worker  = compacq.get();

    m_compBuffer.erase(m_compBuffer.begin(), m_compBuffer.end());
    worker->setOutput(m_compBuffer.data(), m_compBuffer.size());

    bool compr_done = false;

    ChunkCompressor ccompr(worker, chunk);

    do {
      ccompr.feed();

      switch (worker->tick()) {
        case IZLibPP::MoreOut: {
          m_compBuffer.resize(m_compBuffer.size() + START_COMP_BUF_SZ);
          worker->setOutput(m_compBuffer.data(), m_compBuffer.size());
        } break;
        case IZLibPP::Done: {
          compr_done = true;
        } break;

        default: break;
      }
    } while (!compr_done);

    return worker->getTotalOutput();
  }

  bool writeComprDataToSector(uint32_t snum, uint32_t length) {
    PayloadHeader hdr = {length, CompressionType::Zlib};
    moveWriteCursorToSector(snum);
    m_file.write(reinterpret_cast<const char*>(&hdr), sizeof(PayloadHeader));
    m_file.write(m_compBuffer.data(), length);
    m_file.flush();
    return true;
  }

  bool writeToSector(uint32_t snum, Chunk& chunk) { // uncompressed writing, do not use! Region files big af
    moveWriteCursorToSector(snum);
    PayloadHeader hdr = {sizeof(Chunk), CompressionType::UncompressedChunk};
    m_file.write(reinterpret_cast<const char*>(&hdr), sizeof(PayloadHeader));
    m_file.write(reinterpret_cast<const char*>(chunk.m_blocks.data()), chunk.m_blocks.size());
    m_file.write(reinterpret_cast<const char*>(chunk.m_meta.data()), chunk.m_meta.size());
    m_file.write(reinterpret_cast<const char*>(chunk.m_light.data()), chunk.m_light.size());
    m_file.write(reinterpret_cast<const char*>(chunk.m_sky.data()), chunk.m_sky.size());
    m_file.flush();
    return true;
  }

  void moveWriteCursorToSector(uint32_t snum) { m_file.seekp(snum * SECTOR_SIZE, std::ios::beg); }

  bool readFromSector(uint32_t snum, Chunk& chunk) { // todo decompression
    m_file.seekg(snum * SECTOR_SIZE, std::ios::beg);
    PayloadHeader hdr;
    m_file.read(reinterpret_cast<char*>(&hdr), sizeof(PayloadHeader));
    switch (hdr.compression) {
      case CompressionType::UncompressedChunk: {
        m_file.read(reinterpret_cast<char*>(chunk.m_blocks.data()), chunk.m_blocks.size());
        m_file.read(reinterpret_cast<char*>(chunk.m_meta.data()), chunk.m_meta.size());
        m_file.read(reinterpret_cast<char*>(chunk.m_light.data()), chunk.m_light.size());
        m_file.read(reinterpret_cast<char*>(chunk.m_sky.data()), chunk.m_sky.size());
      } break;
      case CompressionType::Zlib: {
        auto dacq   = g_decom.acquire();
        auto worker = dacq.get();

        if (m_compBuffer.size() < START_COMP_BUF_SZ) m_compBuffer.resize(START_COMP_BUF_SZ);

        bool decomp_done = false;

        ChunkDecompressor cdcomp(worker, chunk);

        do {
          if (worker->getAvailableInput() == 0) {
            auto rd = std::min(uint32_t(m_compBuffer.size()), hdr.length);
            if (rd > 0) {
              hdr.length -= rd;
              m_file.read(m_compBuffer.data(), rd);
              worker->setInput(m_compBuffer.data(), rd);
            }
          }

          switch (worker->tick()) {
            case IZLibPP::MoreOut: {
              cdcomp.feed();
            } break;
            case IZLibPP::Done: {
              decomp_done = true;
            } break;

            default: break;
          }
        } while (!decomp_done);
      } break;

      default: {
        throw UnsupportedCompression(hdr.compression);
      } break;
    }
    return true;
  }

  static uint32_t getIndex(const IntVector2& pos) { return (pos.x & 31) + (pos.z & 31) * 32; }

  void setOffset(const IntVector2& pos, RegOff offset) {
    auto idx = getIndex(pos);
    m_file.seekp(idx * sizeof(RegOff), std::ios::beg);
    m_file.write((char*)&(m_offsets[idx] = offset), sizeof(RegOff));
    m_file.flush();
  }

  std::chrono::system_clock::time_point m_lastAccess;
  std::vector<bool>                     m_sectorFree;
  std::array<RegOff, OFFSET_TAB_SZ>     m_offsets;
  std::fstream                          m_file;
  std::vector<char>                     m_compBuffer;
};

std::unique_ptr<IRegionFile> createRegionFile(const std::string& fname) {
  return std::make_unique<RegionFile>(fname);
}
