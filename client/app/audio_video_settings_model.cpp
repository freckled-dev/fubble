#include "audio_video_settings_model.hpp"
#include "client/audio_device_settings.hpp"
#include "client/ui/frame_provider_google_video_device.hpp"
#include "client/video_settings.hpp"
#include "error_model.hpp"
#include "rtc/google/audio_devices.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/capture/video/enumerator.hpp"

using namespace client;

namespace {
class video_devices_model : public devices_model {
public:
  video_devices_model(rtc::google::capture::video::enumerator &enumerator,
                      QObject *parent)
      : devices_model(parent), enumerator(enumerator) {
    refresh();
  }

  void refresh() {
    devices = enumerator.enumerate();
    update_available(!devices.empty());
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

  rtc::google::capture::video::enumerator &enumerator;
  std::vector<rtc::google::capture::video::information> devices;
};
class audio_devices_model : public devices_model {
public:
  audio_devices_model(rtc::google::audio_devices &audio_devices,
                      QObject *parent)
      : devices_model(parent), audio_devices(audio_devices) {}

  int rowCount([[maybe_unused]] const QModelIndex &parent =
                   QModelIndex()) const override {
    return std::max<int>(devices.size(), 1);
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
      : audio_devices_model(audio_devices, parent) {
    refresh();
  }

  void refresh() {
    devices = audio_devices.get_recording_devices();
    update_available(!devices.empty());
  }

protected:
};
class output_audio_devices_model : public audio_devices_model {
public:
  output_audio_devices_model(rtc::google::audio_devices &audio_devices,
                             client::audio_device_settings &, QObject *parent)
      : audio_devices_model(audio_devices, parent) {
    refresh();
  }

  void refresh() {
    devices = audio_devices.get_playout_devices();
    update_available(!devices.empty());
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
    rtc::google::capture::video::enumerator &video_device_enumerator,
    rtc::google::capture::video::device_factory &video_device_factory,
    client::audio_device_settings &audio_settings,
    video_settings &video_settings_, error_model &error_model_, QObject *parent)
    : QObject(parent), video_device_enumerator(video_device_enumerator),
      audio_settings(audio_settings), video_settings_(video_settings_),
      video_device_factory(video_device_factory), error_model_(error_model_) {
  audio_devices.enumerate();
  output_devices =
      new output_audio_devices_model(audio_devices, audio_settings, this);
  input_devices = new recording_audio_devices_model(audio_devices, this);
  auto video_devices_uncasted =
      new video_devices_model(video_device_enumerator, this);
  video_devices = video_devices_uncasted;
  audio_output_device_index = audio_settings.get_playout_device();
  audio_input_device_index = audio_settings.get_recording_device();
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
    return;
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
  } catch (const boost::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "could not change video device";
    video_device.reset();
    video.reset();
    error_model_.set_error(error_model::type::failed_to_start_camera, error);
  }
  video_changed(video.get());
}

void audio_video_settings_model::update_video_device_index() {
  auto id_optional = video_settings_.get_device_id();
  if (!id_optional) {
    return;
  }
  auto id = id_optional.value();
  auto devices = video_device_enumerator.enumerate();
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
