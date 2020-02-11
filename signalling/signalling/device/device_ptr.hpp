#ifndef UUID_396EC8D9_4DFE_4F15_928D_A7B1A2F3FAAE
#define UUID_396EC8D9_4DFE_4F15_928D_A7B1A2F3FAAE

#include <memory>

namespace signalling::device {
class device;
// TODO refactor to `unique_ptr`
using device_ptr = std::shared_ptr<device>;
} // namespace signalling::device

#endif
