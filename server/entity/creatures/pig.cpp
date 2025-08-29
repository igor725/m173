#include "pig.h"

namespace Entities {

class Pig: public IPig {
  public:
  Pig(const DoubleVector3& pos): IPig() { m_position = pos, m_prevPosition = pos; }

  ~Pig() = default;

  void tick(double_t delta) final { refreshMeta(); }

  void readMetadata(PacketWriter::MetaDataStream& mds) const final { mds.putByte(16, m_hasSaddle ? 1 : 0); }

  void setSaddle(bool active) final {
    if (m_hasSaddle == active) return;
    m_hasSaddle = active, m_isMetaUpdated = false;
  }

  private:
  struct /*Flags*/ {
    bool m_hasSaddle : 1 = false;
  };
};

namespace Create {
std::unique_ptr<IPig> pig(const DoubleVector3& pos) {
  return std::make_unique<Pig>(pos);
}
} // namespace Create

} // namespace Entities
