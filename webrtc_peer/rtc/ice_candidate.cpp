#include "ice_candidate.hpp"
#include <fmt/format.h>

std::ostream &rtc::operator<<(std::ostream &out,
                              const rtc::ice_candidate &print) {
  return out << fmt::format("{{mlineindex:{},sdp:'{}'}}", print.mlineindex,
                            print.sdp);
}
