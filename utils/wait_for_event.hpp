#ifndef UUID_3C0D2415_B2F2_46BD_A556_C95AFE1E6E36
#define UUID_3C0D2415_B2F2_46BD_A556_C95AFE1E6E36

#include <mutex>

class wait_for_event {
public:
  wait_for_event() { mutex.lock(); }
  void wait() { std::unique_lock<std::mutex> end(mutex); }
  void event() { mutex.unlock(); }

protected:
  std::mutex mutex;
};

#endif
