#include "connection.hpp"
#include "logging/logger.hpp"
#include <boost/thread/executors/executor_adaptor.hpp>
#include <gtest/gtest.h>
#include <queue>
extern "C" {
#include <gst/gst.h>
}

namespace {
struct glib_executor {
  using callback = std::function<void()>;
  std::queue<callback> callbacks;
  void submit(callback &&call) {
    callbacks.emplace(std::move(call));
    g_idle_add(&glib_executor::called_by_glib, this);
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
    BOOST_ASSERT(!self->callbacks.empty());
    auto &call = self->callbacks.front();
    call();
    self->callbacks.pop();
    return false;
  }
};
} // namespace

struct GstreamerConnection : ::testing::Test {
  GMainLoop *main_loop = g_main_loop_new(nullptr, false);
  boost::executor_adaptor<glib_executor> executor;
  rtc::gstreamer::connection connection;
  ~GstreamerConnection() { g_main_loop_unref(main_loop); }
};

TEST_F(GstreamerConnection, SetUp) {
  EXPECT_EQ(connection.get_state(), rtc::gstreamer::connection::state::new_);
  EXPECT_EQ(connection.get_signalling_state(),
            rtc::gstreamer::connection::signalling_state::stable);
}

TEST_F(GstreamerConnection, CreateOffer) {
  auto offer = connection.create_offer();
  bool called{};
  logging::logger logger;
  offer.then(executor, [&](auto result) {
    auto description = result.get();
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "got session_description, sdp:" << description.sdp;
    EXPECT_FALSE(description.sdp.empty());
    g_main_loop_quit(main_loop);
    called = true;
  });
  g_main_loop_run(main_loop);
  EXPECT_TRUE(called);
}

TEST_F(GstreamerConnection, SetLocalDescription) {
  bool called{};
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
  g_main_loop_run(main_loop);
  EXPECT_TRUE(called);
}

TEST_F(GstreamerConnection, SetInvalidDescription) {
  rtc::session_description invalid;
  invalid.type_ = rtc::session_description::type::answer;
  invalid.sdp = "";
  bool called{};
  connection.set_local_description(invalid).then([&](auto result) {
    EXPECT_TRUE(result.has_exception());
    called = true;
    g_main_loop_quit(main_loop);
  });
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
  g_main_loop_run(main_loop);
  EXPECT_TRUE(called);
}
#endif
