#include "audio_video_settings_model.hpp"
#include "audio_video_settings_devices_model.hpp"
#include "error_model.hpp"
#include "fubble/client/ui/frame_provider_google_video_device.hpp"
#include "fubble/client/video_settings.hpp"
#include "fubble/utils/timer.hpp"

using namespace client;

devices_model::devices_model(QObject *parent) : QAbstractListModel(parent) {}

void devices_model::update_available(const bool new_available) {
  if (new_available == available)
    return;
  available = new_available;
  available_changed(available);
}

QHash<int, QByteArray> devices_model::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[id_role] = "inputId";
  roles[name_role] = "name";
  return roles;
}

audio_video_settings_model::audio_video_settings_model(
    std::shared_ptr<rtc::audio_devices> audio_devices,
    std::shared_ptr<rtc::video_devices> video_device_enumerator,
    std::shared_ptr<rtc::video_device_factory> video_device_factory,
    client::audio_device_settings &audio_settings,
    video_settings &video_settings_, error_model &error_model_,
    std::shared_ptr<utils::timer_factory> timer_factory, QObject *parent)
    : QObject(parent), audio_devices(audio_devices),
      video_device_enumerator(video_device_enumerator),
      audio_settings(audio_settings), video_settings_(video_settings_),
      video_device_factory(video_device_factory),
      error_model_(error_model_), timer_factory{timer_factory} {
  // audio
  audio_devices_timer =
      timer_factory->create_interval_timer(std::chrono::seconds(1));
  output_devices =
      new output_audio_devices_model(audio_devices, audio_settings, this);
  input_devices = new recording_audio_devices_model(audio_devices, this);
  update_audio_devices();
  connect(this, &audio_video_settings_model::update_audio_devices_changed, this,
          &audio_video_settings_model::on_enable_update_audio_devices);

  // video
  auto video_devices_uncasted =
      new video_devices_model(video_device_enumerator, this);
  video_devices = video_devices_uncasted;
  update_video_device_index();
  if (video_devices_uncasted->has_devices())
    onVideoDeviceActivated(video_device_index);
}

audio_video_settings_model::~audio_video_settings_model() = default;

void audio_video_settings_model::onAudioInputDeviceActivated(int index) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", index:" << index;
  audio_settings.set_recording_device(index);
}

void audio_video_settings_model::update_audio_devices() {
  audio_devices->enumerate();
  output_devices->refresh();
  input_devices->refresh();
  auto new_audio_output_device_index = audio_settings.get_playout_device();
  if (new_audio_output_device_index != audio_output_device_index) {
    audio_output_device_index = new_audio_output_device_index;
    audio_output_device_index_changed(audio_output_device_index);
  }
  auto new_audio_input_device_index = audio_settings.get_recording_device();
  if (new_audio_input_device_index != audio_input_device_index) {
    audio_input_device_index = new_audio_input_device_index;
    audio_input_device_index_changed(audio_input_device_index);
  }
}

void audio_video_settings_model::on_enable_update_audio_devices(bool enabled) {
  if (enabled) {
    audio_devices_timer->start([this] { update_audio_devices(); });
    return;
  }
  audio_devices_timer->stop();
}

void audio_video_settings_model::onAudioOutputDeviceActivated(int index) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", index:" << index
      << ", count:" << output_devices->rowCount();
  audio_settings.set_output_device(index);
}

void audio_video_settings_model::onVideoDeviceActivated(int index) {
  auto video_devices_casted =
      dynamic_cast<video_devices_model *>(video_devices);
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", index:" << index;
  if (!video_devices_casted->has_devices())
    return reset_video();
  auto rowCount = video_devices->rowCount();
  if (index >= rowCount) {
    BOOST_LOG_SEV(logger, logging::severity::error) << "index >= rowCount()";
    BOOST_ASSERT(false);
    return;
  }
  bool was_playing{};
  if (video) {
    was_playing = video->get_playing();
    video.reset();
  }
  const auto id = video_devices_casted->get_id_by_index(index);
  try {
    video_settings_.change_to_device(id);
    video_device = video_device_factory->create(id);
    video = std::make_unique<ui::frame_provider_google_video_device>(
        *std::dynamic_pointer_cast<rtc::google::capture::video::device>(
            video_device),
        nullptr);
    if (was_playing)
      video->play();
    video_changed(video.get());
  } catch (const boost::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "could not change video device";
    reset_video();
    error_model_.set_error(error_model::type::failed_to_start_camera, error);
  }
}

void audio_video_settings_model::reset_video() {
  video_device.reset();
  video.reset();
  video_changed(video.get());
}

void audio_video_settings_model::update_video_device_index() {
  auto id_optional = video_settings_.get_device_id();
  if (!id_optional) {
    return;
  }
  auto id = id_optional.value();
  auto devices = video_device_enumerator->get_enumerated();
  auto found = std::find_if(devices.cbegin(), devices.cend(),
                            [&](const auto &check) { return check.id == id; });
  if (found == devices.cend())
    return;
  video_device_index = std::distance(devices.cbegin(), found);
}

ui::frame_provider_google_video_device *
audio_video_settings_model::get_video() {
  return video.get();
}
