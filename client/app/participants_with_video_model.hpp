#ifndef UUID_FC8EB317_730A_47D7_999E_DFF57083EA87
#define UUID_FC8EB317_730A_47D7_999E_DFF57083EA87

#include "participants_model.hpp"

namespace client {
class participants_with_video_model : public participants_model {
  Q_OBJECT
public:
  using participants_model::participants_model;

protected:
  std::vector<participant *>
  filter_joining(const std::vector<participant *> &joining) override;
};
} // namespace client

#endif
