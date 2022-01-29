#include <boost/outcome/std_result.hpp>

namespace fubble::outcome {
template <class R> using result = boost::outcome_v2::std_result<R>;
}
