#include "severity.hpp"
#include <boost/assert.hpp>

std::ostream &logging::operator<<(std::ostream &out,
                                  const logging::severity print) {
  switch (print) {
  case trace:
    return out << "trace";
  case debug:
    return out << "debug";
  case info:
    return out << "info";
  case warning:
    return out << "warning";
  case error:
    return out << "error";
  case fatal:
    return out << "fatal";
  }
  BOOST_ASSERT(false);
  return out;
}

std::istream &logging::operator>>(std::istream &in, logging::severity &set) {
  std::string word;
  in >> word;
  if (word == "trace")
    set = trace;
  if (word == "debug")
    set = debug;
  if (word == "info")
    set = info;
  if (word == "warning")
    set = warning;
  if (word == "error")
    set = error;
  if (word == "fatal")
    set = fatal;
  return in;
}
