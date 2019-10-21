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

TEST_F(GstreamerConnection, SetUp) {}

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
