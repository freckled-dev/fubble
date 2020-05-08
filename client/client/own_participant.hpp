#ifndef UUID_E1835F9B_FE3B_4435_9CB4_0660FB771C06
#define UUID_E1835F9B_FE3B_4435_9CB4_0660FB771C06

#include "participant.hpp"

namespace client {
class own_media;
class own_participant : public participant {
public:
  own_participant(matrix::user &matrix_participant, own_media &own_media_);
  ~own_participant();
  boost::future<void> close() override;

protected:
  videos_type get_videos() const override;

  own_media &own_media_;
};
} // namespace client

#endif
