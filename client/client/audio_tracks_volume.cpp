#include "audio_tracks_volume.hpp"
#include "client/add_audio_to_connection.hpp"
#include "client/participant.hpp"
#include "client/participants.hpp"
#include "client/room.hpp"
#include "client/rooms.hpp"
#include "client/tracks_adder.hpp"
#include "rtc/google/audio_track.hpp"

using namespace client;

namespace {
class audio_tracks_volume_impl : public audio_tracks_volume {
public:
  audio_tracks_volume_impl(rooms &rooms_, tracks_adder &tracks_adder_,
                           add_audio_to_connection &audio_track_adder)
      : rooms_(rooms_), tracks_adder_(tracks_adder_),
        audio_track_adder(audio_track_adder) {
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
    update_all_participants();
    update_self_muted();
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
  }

  bool get_deafen() override { return deafned; }

protected:
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

  void update_audio(const std::string &id, rtc::google::audio_track &audio) {
    if (!room_)
      return;
    if (id == room_->get_own_id())
      return; // enabling own audio would result in loopback audio!
    const bool enabled = !(muted_all_except_self || deafned);
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", id:" << id << ", enabled:" << enabled
        << ", muted_self:" << muted_self
        << ", muted_all_except_self:" << muted_all_except_self
        << ", deafned:" << deafned;
    audio.set_enabled(enabled);
  }

  void update_self_muted() {
    const bool audio_track_added_target = !(muted_self || deafned);
    if (audio_track_added == audio_track_added_target)
      return;
    audio_track_added = audio_track_added_target;
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", audio_track_added:" << audio_track_added;
    audio_track_adder.get_track()->set_enabled(audio_track_added);
#if 0
    if (audio_track_added)
      return tracks_adder_.add(audio_track_adder);
    tracks_adder_.remove(audio_track_adder);
#endif
  }

#if 0 // TODO
  struct audio_setting {
    std::string participant_id;
    bool muted;
    double volume;
  };
#endif

  client::logger logger{"audio_tracks_volume_impl"};
  rooms &rooms_;
  tracks_adder &tracks_adder_;
  add_audio_to_connection &audio_track_adder;
  std::shared_ptr<room> room_;
  participants *participants_{};
  bool muted_all_except_self{};
  bool muted_self{};
  bool deafned{};
  bool audio_track_added{};
};
} // namespace

std::unique_ptr<audio_tracks_volume>
audio_tracks_volume::create(rooms &rooms_, tracks_adder &tracks_adder_,
                            add_audio_to_connection &audio_track_adder) {
  return std::make_unique<audio_tracks_volume_impl>(rooms_, tracks_adder_,
                                                    audio_track_adder);
}
