#include "audio_video_settings_model.hpp"
#include "client/audio_device_settings.hpp"
#include "client/ui/frame_provider_google_video_device.hpp"
#include "client/video_settings.hpp"
#include "error_model.hpp"
#include "rtc/google/audio_devices.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/capture/video/enumerator.hpp"
#include "utils/timer.hpp"

using namespace client;

namespace {
class video_devices_model : public devices_model {
public:
  video_devices_model(rtc::video_devices &enumerator, QObject *parent)
      : devices_model(parent), enumerator(enumerator) {
    refresh();
    enumerator.on_enumerated_changed.connect([this] { refresh(); });
  }

  void refresh() override {
    auto devices_cache = enumerator.get_enumerated();
    if (devices_cache == devices)
      return;
    beginResetModel();
    devices = enumerator.get_enumerated();
    update_available(!devices.empty());
    endResetModel();
  }

  int rowCount([[maybe_unused]] const QModelIndex &parent =
                   QModelIndex()) const override {
    return std::max<int>(devices.size(), 1);
  }

  QVariant data(const QModelIndex &index, int role) const override {
    if (!available)
      return tr("There are no video devices available");
    const int row = index.row();
    const std::size_t device_index = static_cast<std::size_t>(row);
    BOOST_ASSERT(device_index < devices.size());
    auto &device = devices[device_index];
    if (role == id_role)
      return QString::fromStdString(device.id);
    BOOST_ASSERT(role == name_role);
    return QString::fromStdString(device.name);
  }

  bool has_devices() { return !devices.empty(); }

  std::string get_id_by_index(int index) {
    BOOST_ASSERT(index >= 0);
    BOOST_ASSERT(index < static_cast<int>(devices.size()));
    return devices[index].id;
  }

  rtc::video_devices &enumerator;
  std::vector<rtc::google::capture::video::information> devices;
};
class audio_devices_model : public devices_model {
public:
  audio_devices_model(rtc::google::audio_devices &audio_devices,
                      QObject *parent)
      : devices_model(parent), audio_devices(audio_devices) {}

  int rowCount([[maybe_unused]] const QModelIndex &parent =
                   QModelIndex()) const override {
    return devices.size();
  }
  QVariant data(const QModelIndex &index, int role) const override {
    if (!available)
      return tr("There are no audio devices available");
    const int row = index.row();
    const std::size_t device_index = static_cast<std::size_t>(row);
    BOOST_ASSERT(device_index < devices.size());
    auto &device = devices[device_index];
    if (role == id_role)
      return device.index;
    BOOST_ASSERT(role == name_role);
    return QString::fromStdString(device.name);
  }

  rtc::google::audio_devices &audio_devices;
  rtc::google::audio_devices::devices devices;
};
class recording_audio_devices_model : public audio_devices_model {
public:
  recording_audio_devices_model(rtc::google::audio_devices &audio_devices,
                                QObject *parent)
      : audio_devices_model(audio_devices, parent) {}

  void refresh() override {
    auto new_devices = audio_devices.get_recording_devices();
    if (new_devices == devices)
      return;
    beginResetModel();
    devices = new_devices;
    endResetModel();
    update_available(true);
  }

protected:
};
class output_audio_devices_model : public audio_devices_model {
public:
  output_audio_devices_model(rtc::google::audio_devices &audio_devices,
                             client::audio_device_settings &, QObject *parent)
      : audio_devices_model(audio_devices, parent) {}

  void refresh() override {
    auto new_devices = audio_devices.get_playout_devices();
    if (new_devices == devices)
      return;
    beginResetModel();
    devices = new_devices;
    endResetModel();
    update_available(true);
  }

protected:
};
} // namespace

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
    rtc::google::audio_devices &audio_devices,
    rtc::video_devices &video_device_enumerator,
    rtc::google::capture::video::device_factory &video_device_factory,
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
  audio_devices.enumerate();
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
  auto video_devices_casted = static_cast<video_devices_model *>(video_devices);
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
    video_device = video_device_factory.create(id);
    video = std::make_unique<ui::frame_provider_google_video_device>(
        *video_device, nullptr);
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
  auto devices = video_device_enumerator.get_enumerated();
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
