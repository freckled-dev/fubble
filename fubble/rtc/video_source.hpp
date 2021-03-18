#ifndef UUID_540C3C00_41CE_4548_8C0E_8191DC904C77
#define UUID_540C3C00_41CE_4548_8C0E_8191DC904C77

namespace rtc {
class video_source {
public:
  virtual ~video_source() = default;
};

class video_source_noop : public video_source {
public:
};
} // namespace rtc

#endif
