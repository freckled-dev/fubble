#ifndef UUID_E8851DC6_97FD_4C70_B8A4_A8406D77F5DD
#define UUID_E8851DC6_97FD_4C70_B8A4_A8406D77F5DD

#include <boost/program_options.hpp>

namespace utils {
struct option_adder {
  boost::program_options::options_description &options;
  template <class value_type>
  void add(std::string flag, value_type &value, std::string description) {
    namespace bpo = boost::program_options;
    options.add_options()(flag.c_str(),
                          bpo::value<value_type>(&value)->default_value(value),
                          description.c_str());
  }
};
} // namespace utils

#endif
