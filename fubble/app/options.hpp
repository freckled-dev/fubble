#ifndef UUID_3CD9552E_58E8_4F8B_96C4_0A0EC07C783B
#define UUID_3CD9552E_58E8_4F8B_96C4_0A0EC07C783B

#include <boost/program_options/options_description.hpp>
#include <fubble/app/core_module.hpp>
#include <fubble/client/log_module.hpp>
#include <fubble/v4l2_hw_h264/config.hpp>

namespace fubble::client::options {
boost::program_options::options_description
create(::client::log_module::config &config);
boost::program_options::options_description
create(::client::core_module::config &config);
boost::program_options::options_description
create(v4l2_hw_h264::config &config);
} // namespace fubble::client::options

#endif
