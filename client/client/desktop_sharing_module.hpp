#ifndef UUID_B05CFCC0_C2D9_4C48_BFBF_F2206126E864
#define UUID_B05CFCC0_C2D9_4C48_BFBF_F2206126E864

#include "client/session_module.hpp"
#include "client/video_module.hpp"

namespace client {
class desktop_sharing;
class desktop_sharing_previews;
class FUBBLE_PUBLIC desktop_sharing_module {
public:
  desktop_sharing_module(
      std::shared_ptr<utils::executor_module> executor_module,
      std::shared_ptr<session_module> session_module_,
      std::shared_ptr<video_module> video_module_);

  std::shared_ptr<desktop_sharing> get_desktop_sharing();
  std::shared_ptr<desktop_sharing_previews> get_desktop_sharing_previews();

protected:
  std::shared_ptr<utils::executor_module> executor_module;
  std::shared_ptr<session_module> session_module_;
  std::shared_ptr<video_module> video_module_;

  std::shared_ptr<desktop_sharing> desktop_sharing_;
  std::shared_ptr<desktop_sharing_previews> desktop_sharing_previews_;
};
} // namespace client

#endif
