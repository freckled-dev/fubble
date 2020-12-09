#ifndef UUID_359FE6BC_8226_4460_8FB3_959DECA38C39
#define UUID_359FE6BC_8226_4460_8FB3_959DECA38C39

#include "fubble/client/logger.hpp"
#include "participant.hpp"

namespace client {
class bot_participant : public participant {
public:
  bot_participant(matrix::user &matrix_participant);
  ~bot_participant();

  boost::future<void> close() override;

protected:
  videos_type get_videos() const override;
  audios_type get_audios() const override;

  client::logger logger{"bot_participant"};
};
} // namespace client

#endif
