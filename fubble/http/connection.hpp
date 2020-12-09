#ifndef UUID_2E061B50_8777_4620_956D_1B9B1CB48279
#define UUID_2E061B50_8777_4620_956D_1B9B1CB48279

namespace http {
class connection {
public:
  virtual ~connection() = default;
  virtual void shutdown() = 0;
  virtual void cancel() = 0;
};
} // namespace http

#endif
