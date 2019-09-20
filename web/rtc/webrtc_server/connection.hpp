#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <functional>

class connection {
 public:
  using callback_message_type = std::function<void(std::string)>;

  connection();
  virtual ~connection();
  connection(const connection&) = delete;

  virtual void read_async(const callback_message_type& callback_message) = 0;

  using message_type = std::vector<char>;
  virtual void write(const message_type& message) = 0;
};

#endif  // CONNECTION_HPP
