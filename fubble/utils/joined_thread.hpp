#ifndef UUID_B719BBE9_84D7_4F57_935B_292460E24D4D
#define UUID_B719BBE9_84D7_4F57_935B_292460E24D4D

#include <thread>
#include <utility>

// TODO move to `std::jthread` when the project moves to C++20
// https://en.cppreference.com/w/cpp/thread/jthread

class joined_thread : public std::thread {
public:
  template <typename call>
  joined_thread(call &&call_)
      : std::thread(std::forward<decltype(call_)>(call_)) {}

  ~joined_thread() {
    if (joinable())
      join();
  }
};

#endif
