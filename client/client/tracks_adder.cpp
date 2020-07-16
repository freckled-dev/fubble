#include "tracks_adder.hpp"

using namespace client;

void tracks_adder::add(track_adder &adder) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  BOOST_ASSERT(find_adder(adder) == adders.cend());
  adders.emplace_back(&adder);
  for (auto connection : connections_)
    adder.add_to_connection(*connection);
}

void tracks_adder::remove(track_adder &adder) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  auto found = find_adder(adder);
  BOOST_ASSERT(found != adders.cend());
  for (auto connection : connections_)
    adder.remove_from_connection(*connection);
  adders.erase(found);
}

void tracks_adder::add_connection(rtc::connection &connection) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "adding " << adders.size()
      << " tracks to the connection:" << &connection;
  BOOST_ASSERT(find_connection(connection) == connections_.cend());
  for (auto &adder : adders)
    adder->add_to_connection(connection);
}

void tracks_adder::remove_connection(rtc::connection &connection) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  auto found = find_connection(connection);
  BOOST_ASSERT(found != connections_.cend());
  for (auto &adder : adders)
    adder->remove_from_connection(connection);
  connections_.erase(found);
}

tracks_adder::connections::iterator
tracks_adder::find_connection(rtc::connection &connection) {
  return std::find_if(connections_.begin(), connections_.end(),
                      [&](auto check) { return check == &connection; });
}

tracks_adder::tracks::iterator
tracks_adder::find_adder(const track_adder &adder) {
  return std::find_if(adders.begin(), adders.end(),
                      [&](auto check) { return check == &adder; });
}
