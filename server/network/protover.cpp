#include "protover.h"

namespace Packet {
uint16_t getProtoVersion() {
#ifdef M173_BETA18_PROTO
  return 17;
#else
  return 14;
#endif
}
} // namespace Packet
