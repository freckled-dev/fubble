#include "connection.hpp"
#include "logging/logger.hpp"
#include "rtc/gstreamer/video_track.hpp"
#include <boost/signals2/connection.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <queue>
#include <thread>
extern "C" {
#include <gst/gst.h>
}

namespace {
struct glib_executor {
  using callback = std::function<void()>;
  struct callback_with_id {
    guint id;
    callback callback_;
  };
  std::queue<callback_with_id> callbacks;
  logging::logger logger;
  std::mutex mutex;
  ~glib_executor() {
    if (callbacks.empty())
      return;
    BOOST_LOG_SEV(logger, logging::severity::warning) << fmt::format(
        "not all callbacks got called. still outstanding amount: '{}'",
        callbacks.size());
    while (!callbacks.empty()) {
      auto &call = callbacks.front();
      g_source_remove(call.id);
      callbacks.pop();
    }
  }
  void submit(callback &&call) {
    std::lock_guard<std::mutex> guard(mutex);
    const auto id = g_idle_add(&glib_executor::called_by_glib, this);
    BOOST_ASSERT(id > 0);
    callbacks.push({id, std::move(call)});
  }
  void close() { BOOST_ASSERT(false); }
  bool closed() {
    BOOST_ASSERT(false);
    return false;
  }
  bool try_executing_one() {
    BOOST_ASSERT(false);
    return false;
  }
  static gboolean called_by_glib(gpointer user_data) {
    auto self = static_cast<glib_executor *>(user_data);
    callback to_call;
    {
      std::lock_guard<std::mutex> guard(self->mutex);
      BOOST_ASSERT(!self->callbacks.empty());
      to_call = std::move(self->callbacks.front().callback_);
      self->callbacks.pop();
    }
    to_call();
    return false;
  }
};
} // namespace

using executor_type = boost::executor_adaptor<glib_executor>;

struct GstreamerConnection : ::testing::Test {
  logging::logger logger;
  GMainLoop *main_loop = g_main_loop_new(nullptr, false);
  executor_type executor;
  rtc::gstreamer::connection connection{executor};
  ~GstreamerConnection() { g_main_loop_unref(main_loop); }
};

struct create_offer_and_set_local_description {
  rtc::gstreamer::connection &connection;
  executor_type &executor;
  boost::promise<void> promise;
  boost::signals2::scoped_connection negotiation_needed_connection;
  create_offer_and_set_local_description(rtc::gstreamer::connection &connection,
                                         executor_type &executor)
      : connection(connection), executor(executor) {
    negotiation_needed_connection = connection.on_negotiation_needed.connect(
        [this] { negotiation_needed(); });
  }
  boost::future<void> operator()() { return promise.get_future(); }
  void negotiation_needed() {
    negotiation_needed_connection.disconnect();
    connection.create_offer()
        .then(executor,
              [this](auto result) {
                return connection.set_local_description(result.get());
              })
        .unwrap()
        .then(executor, [this](auto) { promise.set_value(); });
  }
};

TEST_F(GstreamerConnection, SetUp) {
  EXPECT_EQ(connection.get_state(), rtc::gstreamer::connection::state::new_);
  EXPECT_EQ(connection.get_signalling_state(),
            rtc::gstreamer::connection::signalling_state::stable);
}

TEST_F(GstreamerConnection, NegotiationNeeded) {
  bool called{};
  connection.on_negotiation_needed.connect([&] {
    called = true;
    g_main_loop_quit(main_loop);
  });
  connection.run();
  g_main_loop_run(main_loop);
  EXPECT_TRUE(called);
}

TEST_F(GstreamerConnection, CreateOffer) {
  bool called{};
  connection.on_negotiation_needed.connect([&] {
    connection.create_offer().then(executor, [&](auto result) {
      auto description = result.get();
      BOOST_LOG_SEV(logger, logging::severity::info)
          << "got session_description, sdp:" << description.sdp;
      EXPECT_FALSE(description.sdp.empty());
      g_main_loop_quit(main_loop);
      called = true;
    });
  });
  connection.run();
  g_main_loop_run(main_loop);
  EXPECT_TRUE(called);
}

TEST_F(GstreamerConnection, SetLocalDescription) {
  bool called{};
  connection.on_negotiation_needed.connect([&] {
    connection.create_offer()
        .then(executor,
              [&](auto result) -> boost::future<void> {
                return connection.set_local_description(result.get());
              })
        .unwrap()
        .then(executor, [&](auto result) {
          EXPECT_NO_THROW(result.get());
          EXPECT_EQ(
              connection.get_signalling_state(),
              rtc::gstreamer::connection::signalling_state::have_local_offer);
          g_main_loop_quit(main_loop);
          called = true;
        });
  });
  connection.run();
  g_main_loop_run(main_loop);
  EXPECT_TRUE(called);
}

TEST_F(GstreamerConnection, SetInvalidDescription) {
  rtc::session_description invalid;
  invalid.type_ = rtc::session_description::type::answer;
  invalid.sdp = "";
  bool called{};
  connection.on_negotiation_needed.connect([&] {
    connection.set_local_description(invalid).then([&](auto result) {
      BOOST_LOG_SEV(logger, logging::severity::info)
          << "did set local description";
      EXPECT_TRUE(result.has_exception());
      called = true;
      g_main_loop_quit(main_loop);
    });
  });
  connection.run();
  g_main_loop_run(main_loop);
  EXPECT_TRUE(called);
}

#if 0 // gstreamer webrtc does not return errors - only logs them
TEST_F(GstreamerConnection, SetDescriptionTwice) {
  rtc::session_description invalid;
  invalid.type_ = rtc::session_description::type::answer;
  invalid.sdp = "abc";
  bool called{};
  connection.set_local_description(invalid)
      .then([&](auto result) {
        result.get();
        invalid.type_ = rtc::session_description::type::offer;
        return connection.set_local_description(invalid);
      })
      .unwrap()
      .then([&](auto result) {
        EXPECT_TRUE(result.has_exception());
        called = true;
        g_main_loop_quit(main_loop);
      });
  connection.run();
  g_main_loop_run(main_loop);
  EXPECT_TRUE(called);
}
#endif

TEST_F(GstreamerConnection, IceCandidates) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << "thread_id:" << std::this_thread::get_id();
  create_offer_and_set_local_description setup{connection, executor};
  setup();
  std::vector<rtc::ice_candidate> candidates;
  auto connection_on_ice_candidate = connection.on_ice_candidate.connect(
      [&](const rtc::ice_candidate &candidate) {
        if (candidates.empty()) {
          connection.close();
          g_main_quit(main_loop);
        }
        candidates.push_back(candidate);
        auto ice_gathering_state = connection.get_ice_gathering_state();
        auto ice_connection_state = connection.get_ice_connection_state();
        BOOST_LOG_SEV(logger, logging::severity::info)
            << fmt::format("got a candidate, gathering_state:'{}', "
                           "ice_connection_state:'{}', thread_id:'{}'",
                           ice_gathering_state, ice_connection_state,
                           std::this_thread::get_id());
      });
  auto track = std::make_shared<rtc::gstreamer::video_track>();
  connection.add_track(track);
  connection.run();
  g_main_loop_run(main_loop);
  connection_on_ice_candidate.disconnect();
  EXPECT_FALSE(candidates.empty());
}

#if 0
TEST_F(GstreamerConnection, VideoTrack) {
  create_offer_and_set_local_description setup{connection, executor};
  setup();
  connection.run();
  g_main_loop_run(main_loop);
}
#endif

