#include "mute_deaf_communicator.hpp"
#include "client/room.hpp"
#include "matrix/room.hpp"
#include "matrix/room_states.hpp"

using namespace client;

namespace {
class mute_deaf_communicator_impl final : public mute_deaf_communicator {
public:
  mute_deaf_communicator_impl(std::shared_ptr<rooms> rooms_,
                              std::shared_ptr<audio_volume> audio_volume_)
      : rooms_{rooms_}, audio_volume_{audio_volume_} {
    rooms_->on_set.connect([this] { on_room_set(); });
    audio_volume_->on_deafed.connect(
        [this](bool deafed) { on_deafed(deafed); });
    audio_volume_->on_muted.connect([this](bool muted) { on_muted(muted); });
  }

protected:
  void on_deafed(bool deafed) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", deafed:" << deafed;
    if (!room_)
      return;
    auto data = nlohmann::json::object();
    data["deafed"] = deafed;

    auto &states = room_->get_native().get_states();
    matrix::room_states::custom state;
    state.key = "io.fubble.audio_state";
    state.data = data;
    states.set_custom(state).then(
        executor, [this](auto result) { did_set_state(result); });
  }

  void on_muted(bool) {}

  void on_room_set() {
    auto set = rooms_->get();
    BOOST_ASSERT(room_ != set);
    room_ = set;
    if (!room_)
      return;
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
};
} // namespace

std::unique_ptr<mute_deaf_communicator>
mute_deaf_communicator::create(std::shared_ptr<rooms> rooms,
                               std::shared_ptr<audio_volume> audio_volume_) {
  return std::make_unique<mute_deaf_communicator_impl>(rooms, audio_volume_);
}
