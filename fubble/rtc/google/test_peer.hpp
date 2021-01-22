#ifndef UUID_65A65605_E788_40E6_BBD1_900B54689414
#define UUID_65A65605_E788_40E6_BBD1_900B54689414

#include "connection.hpp"
#include "factory.hpp"
#include <boost/thread/executors/inline_executor.hpp>

namespace {
struct test_peer {
  boost::inline_executor executor;
  std::unique_ptr<rtc::google::connection> instance;

  test_peer(rtc::google::factory &creator)
      : instance(creator.create_connection()) {}
  ~test_peer() { instance->close(); }

  boost::shared_future<rtc::session_description> create_offer() {
    return instance->create_offer({});
  }

  boost::future<rtc::session_description> create_answer() {
    return instance->create_answer();
  }

  boost::future<void> set_local_description(
      boost::shared_future<rtc::session_description> description) {
    return description
        .then(executor,
              [this](auto description) {
                return instance->set_local_description(description.get());
              })
        .unwrap();
  }
  boost::future<void> set_remote_description(
      boost::shared_future<rtc::session_description> description) {
    return description
        .then(executor,
              [this](auto description) {
                return instance->set_remote_description(description.get());
              })
        .unwrap();
  }
  void connect_ice_candidates(test_peer &other) {
    instance->on_ice_candidate.connect(
        [&](auto candidate) { other.instance->add_ice_candidate(candidate); });
    other.instance->on_ice_candidate.connect(
        [&](auto candidate) { instance->add_ice_candidate(candidate); });
  }
};
} // namespace

#endif
