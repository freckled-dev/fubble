#include "mute_deaf_communicator.hpp"
#include "fubble/client/room.hpp"
#include "fubble/matrix/room.hpp"
#include "fubble/matrix/room_states.hpp"

using namespace client;

namespace {
class mute_deaf_communicator_impl final
    : public mute_deaf_communicator,
      public std::enable_shared_from_this<mute_deaf_communicator> {
public:
  mute_deaf_communicator_impl(std::shared_ptr<rooms> rooms_,
                              std::shared_ptr<audio_volume> audio_volume_)
      : rooms_{rooms_}, audio_volume_{audio_volume_} {
    rooms_->on_set.connect([this] { on_room_set(); });
    audio_volume_->on_deafed.connect(
        [this](bool deafed) { on_self_deafed(deafed); });
    audio_volume_->on_muted.connect(
        [this](bool muted) { on_self_muted(muted); });
  }

  bool is_deafed(const std::string &id) const override {
    auto found = mutes_and_deafs.find(id);
    if (found == mutes_and_deafs.cend())
      return false;
    return found->second.deafed;
  }

  bool is_muted(const std::string &id) const override {
    auto found = mutes_and_deafs.find(id);
    if (found == mutes_and_deafs.cend())
      return false;
    return found->second.muted;
  }

protected:
  static std::string state_key() {
    static const std::string result = "io.fubble.audio_state";
    return result;
  }

  void on_self_deafed(bool deafed_) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", deafed:" << deafed_;
    self_deafed = deafed_;
    if (!room_)
      return;
    send_state();
  }

  void on_self_muted(bool muted_) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", muted:" << muted_;
    self_muted = muted_;
    if (!room_)
      return;
    send_state();
  }

  void send_state() {
    auto data = nlohmann::json::object();
    data["deafed"] = self_deafed;
    data["muted"] = self_muted;

    auto &states = room_->get_native().get_states();
    matrix::room_states::custom state;
    state.type = state_key();
    state.key = room_->get_own_id();
    state.data = data;
    states.set_custom(state).then(
        executor, [thiz = shared_from_this(), this](auto result) {
          did_set_state(result);
        });
  }

  void on_room_set() {
    auto set = rooms_->get();
    BOOST_ASSERT(room_ != set);
    room_ = set;
    if (!room_) {
      mutes_and_deafs.clear();
      return;
    }
    if (self_deafed || self_muted)
      send_state();
    room_->get_native().get_states().on_custom.connect(
        [this](const auto &custom_) { on_custom(custom_); });
  }

  void on_custom(const matrix::room_states::custom &custom_) {
    if (custom_.type != state_key())
      return;
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", got a " << state_key() << ", key:" << custom_.key
        << ", data:" << custom_.data.dump();
    BOOST_ASSERT(!custom_.key.empty());
    const auto id = custom_.key;
    mute_deaf &change = mutes_and_deafs[id];
    if (custom_.data.contains("deafed")) {
      const bool deafed_changed = custom_.data["deafed"];
      if (change.deafed != deafed_changed) {
        change.deafed = deafed_changed;
        on_deafed(id, change.deafed);
      }
    }
    if (custom_.data.contains("muted")) {
      const bool muted_changed = custom_.data["muted"];
      if (change.muted != muted_changed) {
        change.muted = muted_changed;
        on_muted(id, change.muted);
      }
    }
  }

  void did_set_state(boost::future<void> &result) {
    try {
      result.get();
    } catch (const boost::exception &error) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << __FUNCTION__
          << ", could not set state:" << boost::diagnostic_information(error);
      BOOST_ASSERT(false);
    }
  }

  boost::inline_executor executor;
  client::logger logger{"mute_deaf_communicator_impl"};
  std::shared_ptr<rooms> rooms_;
  std::shared_ptr<audio_volume> audio_volume_;
  std::shared_ptr<room> room_;
  struct mute_deaf {
    bool muted{};
    bool deafed{};
  };
  std::unordered_map<std::string, mute_deaf> mutes_and_deafs;
  bool self_muted{};
  bool self_deafed{};
}; // namespace
} // namespace

std::shared_ptr<mute_deaf_communicator>
mute_deaf_communicator::create(std::shared_ptr<rooms> rooms,
                               std::shared_ptr<audio_volume> audio_volume_) {
  return std::make_shared<mute_deaf_communicator_impl>(rooms, audio_volume_);
}
