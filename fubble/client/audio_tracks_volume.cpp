#include "audio_tracks_volume.hpp"
#include <fubble/client/add_audio_to_connection.hpp>
#include <fubble/client/own_audio_track.hpp>
#include <fubble/client/participant.hpp>
#include <fubble/client/participants.hpp>
#include <fubble/client/room.hpp>
#include <fubble/client/rooms.hpp>
#include <fubble/client/tracks_adder.hpp>
#include <fubble/rtc/audio_track.hpp>

using namespace client;

namespace {
class audio_tracks_volume_impl : public audio_tracks_volume {
public:
  audio_tracks_volume_impl(
      rooms &rooms_, tracks_adder &tracks_adder_,
      const std::shared_ptr<add_audio_to_connection> &audio_track_adder,
      own_audio_track &own_audio_track_)
      : rooms_(rooms_), tracks_adder_(tracks_adder_),
        audio_track_adder(audio_track_adder),
        own_audio_track_(own_audio_track_) {
    on_room(rooms_.get());
    rooms_.on_set.connect([this, &rooms_] { on_room(rooms_.get()); });
    update_self_muted();
    tracks_adder_.add(audio_track_adder);
  }

  ~audio_tracks_volume_impl() {}

  void mute_all_except_self(bool muted_paramter) override {
    if (muted_all_except_self == muted_paramter)
      return;
    muted_all_except_self = muted_paramter;
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", muted_all:" << muted_all_except_self;
    update_all_participants();
  }

  bool get_all_muted_except_self() const override {
    return muted_all_except_self;
  }

  void mute_self(bool muted) override {
    if (muted == muted_self)
      return;
    muted_self = muted;
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", muted_self:" << muted_self;
    update_self_muted();
    on_muted(muted_self);
  }

  bool get_self_muted() override { return muted_self; }

  void deafen(bool deafed_parameter) override {
    if (deafned == deafed_parameter)
      return;
    deafned = deafed_parameter;
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", deafned:" << deafned;
    update_all_participants();
    update_self_muted();
    on_deafed(deafned);
  }

  bool get_deafen() override { return deafned; }

  void set_volume(std::string id, double volume) override {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", id:" << id << ", volume:" << volume;
    auto &set = get_or_add_setting(id);
    set.volume = volume;
    update_participant(id);
  }

  double get_volume(std::string id) const override {
    auto found = find_setting(id);
    if (found == settings.cend())
      return 1.0;
    return found->volume;
  }

  void mute(std::string id, bool muted) override {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", id:" << id << ", muted:" << muted;
    auto &set = get_or_add_setting(id);
    set.muted = muted;
    update_participant(id);
  }

  bool get_muted(std::string id) const override {
    auto found = find_setting(id);
    if (found == settings.cend())
      return false;
    return found->muted;
  }

protected:
  struct audio_setting {
    std::string participant_id;
    bool muted{};
    double volume{1.0};
  };

  std::vector<audio_setting>::iterator find_setting(const std::string &id) {
    return std::find_if(
        settings.begin(), settings.end(),
        [&](const auto &check) { return check.participant_id == id; });
  }

  std::vector<audio_setting>::const_iterator
  find_setting(const std::string &id) const {
    return std::find_if(
        settings.cbegin(), settings.cend(),
        [&](const auto &check) { return check.participant_id == id; });
  }

  audio_setting &get_or_add_setting(const std::string &id) {
    auto found = find_setting(id);
    if (found != settings.end())
      return *found;
    auto &result = settings.emplace_back();
    result.participant_id = id;
    return result;
  }

  void on_room(const std::shared_ptr<room> &room_parameter) {
    if (room_ == room_parameter)
      return;
    room_ = room_parameter;
    if (!room_) {
      participants_ = nullptr;
      return;
    }
    participants_ = &room_->get_participants();
    for (auto participant : participants_->get_all())
      on_participant(*participant);
    update_all_participants();
  }

  void on_participant(participant &participant_) {
    auto id = participant_.get_id();
    participant_.on_audio_added.connect(
        [this, id](auto &audio) { update_audio(id, audio); });
  }

  void update_all_participants() {
    if (!room_)
      return;
    BOOST_ASSERT(participants_ != nullptr);
    for (auto participant_ : participants_->get_all()) {
      auto id = participant_->get_id();
      for (auto &audio : participant_->get_audios())
        update_audio(id, *audio);
    }
  }

  void update_participant(const std::string &id) {
    auto got = participants_->get(id);
    if (!got) {
      BOOST_ASSERT(false);
      return;
    }
    for (auto &audio : got->get_audios())
      update_audio(id, *audio);
  }

  void update_audio(const std::string &id, rtc::audio_track &audio) {
    if (!room_)
      return;
    if (id == room_->get_own_id())
      return; // enabling own audio would result in loopback audio!
    const bool participant_muted = get_muted(id);
    const double volume = get_volume(id);
    const bool enabled =
        !(muted_all_except_self || deafned || participant_muted);
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", id:" << id << ", enabled:" << enabled
        << ", muted_self:" << muted_self
        << ", muted_all_except_self:" << muted_all_except_self
        << ", deafned:" << deafned
        << ", participant_muted:" << participant_muted << ", volume:" << volume;
    audio.set_enabled(enabled);
    if (!enabled)
      return;
    audio.set_volume(volume);
  }

  void update_self_muted() {
    const bool actually_muted_target = muted_self || deafned;
    if (actually_muted == actually_muted_target)
      return;
    actually_muted = actually_muted_target;
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", actually_muted:" << actually_muted;
    own_audio_track_.set_enabled(!actually_muted);
  }

  client::logger logger{"audio_tracks_volume_impl"};
  rooms &rooms_;
  tracks_adder &tracks_adder_;
  const std::shared_ptr<add_audio_to_connection> audio_track_adder;
  own_audio_track &own_audio_track_;
  std::shared_ptr<room> room_;
  participants *participants_{};
  bool muted_all_except_self{};
  bool muted_self{};
  bool deafned{};
  bool actually_muted{};
  std::vector<audio_setting> settings;
};
} // namespace

std::unique_ptr<audio_tracks_volume> audio_tracks_volume::create(
    rooms &rooms_, tracks_adder &tracks_adder_,
    const std::shared_ptr<add_audio_to_connection> &audio_track_adder,
    own_audio_track &own_audio_track_) {
  return std::make_unique<audio_tracks_volume_impl>(
      rooms_, tracks_adder_, audio_track_adder, own_audio_track_);
}
