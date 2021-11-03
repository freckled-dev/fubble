#ifndef UUID_BE3B5C7E_9570_4F7C_A304_A12A219473C9
#define UUID_BE3B5C7E_9570_4F7C_A304_A12A219473C9

namespace rtc {
class video_frame {
public:
  virtual ~video_frame() = default;
  using buffer = std::vector<unsigned char>;
  virtual void i420(buffer &data_y, int &stride_y, buffer &data_u,
                    int &stride_u, buffer &data_v, int &stride_v) const = 0;
  virtual int width() const = 0;
  virtual int height() const = 0;
};
} // namespace rtc

#endif
