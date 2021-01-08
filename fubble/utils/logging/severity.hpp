#ifndef UUID_F82A343B_D46A_49C5_8576_C60B65748821
#define UUID_F82A343B_D46A_49C5_8576_C60B65748821

#include <ostream>
#include <istream>

namespace logging {
enum severity { trace, debug, info, warning, error, fatal };

std::ostream &operator<<(std::ostream &out, const severity print);
std::istream &operator>>(std::istream &in, severity &set);

} // namespace logging

#endif
