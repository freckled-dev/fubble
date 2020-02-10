#ifndef UUID_E1835F9B_FE3B_4435_9CB4_0660FB771C06
#define UUID_E1835F9B_FE3B_4435_9CB4_0660FB771C06

#include "participant.hpp"
#include "session/participant.hpp"

namespace client {
class own_participant : public participant {
public:
  own_participant(const session::participant &session_participant);
  ~own_participant();

  std::string get_id() const override;
  std::string get_name() const override;
  void update(const session::participant &update) override;

protected:
  session::participant session_participant;
};
} // namespace client

#endif
