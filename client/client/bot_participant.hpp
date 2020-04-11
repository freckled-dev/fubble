#ifndef UUID_359FE6BC_8226_4460_8FB3_959DECA38C39
#define UUID_359FE6BC_8226_4460_8FB3_959DECA38C39

#include "client/logger.hpp"
#include "participant.hpp"
#include "rtc/track_ptr.hpp"
#include "session/participant.hpp"

namespace client {
class bot_participant : public participant {
public:
  bot_participant(session::participant &session_participant);
  ~bot_participant();

protected:
  videos_type get_videos() const override;

  client::logger logger{"bot_participant"};
};
} // namespace client

#endif
