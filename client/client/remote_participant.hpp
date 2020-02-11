#ifndef UUID_879851E1_6012_47BE_AF61_ECB95660B186
#define UUID_879851E1_6012_47BE_AF61_ECB95660B186

#include "logging/logger.hpp"
#include "participant.hpp"
#include "rtc/track_ptr.hpp"
#include "session/participant.hpp"

namespace client {
class peer;
class peer_creator;
class remote_participant : public participant {
public:
  remote_participant(std::unique_ptr<peer> peer_,
                     const session::participant &session_participant);
  ~remote_participant();

  std::string get_id() const override;
  std::string get_name() const override;
  void update(const session::participant &update) override;
  videos_type get_videos() const override;

protected:
  void on_track(rtc::track_ptr track);

  logging::logger logger;
  std::unique_ptr<peer> peer_;
  session::participant session_participant;
  videos_type videos;
};
} // namespace client

#endif
