#ifndef UUID_E1835F9B_FE3B_4435_9CB4_0660FB771C06
#define UUID_E1835F9B_FE3B_4435_9CB4_0660FB771C06

#include "client/desktop_sharing.hpp"
#include "client/participant.hpp"

namespace client {
class own_media;
class own_participant : public participant {
public:
  own_participant(matrix::user &matrix_participant, own_media &own_media_);
  ~own_participant();
  boost::future<void> close() override;

protected:
  videos_type get_videos() const override;
  audios_type get_audios() const override;
  videos_type get_screens() const override;

  own_media &own_media_;
  const std::shared_ptr<desktop_sharing> desktop_sharing_;
};
} // namespace client

#endif
