#include <boost/exception/all.hpp>
#include <exception>

namespace utils {
struct exception : virtual boost::exception, virtual std::exception {};
} // namespace utils
