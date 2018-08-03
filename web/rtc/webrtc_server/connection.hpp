#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <functional>

class connection {
 public:
  using callback_message_type = std::function<void(std::string)>;

  connection();
  virtual ~connection();

  virtual void read_async(const callback_message_type& callback_message) = 0;

 protected:
  callback_message_type callback_message;
};

#endif  // CONNECTION_HPP
