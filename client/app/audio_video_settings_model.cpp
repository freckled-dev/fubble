#include "audio_video_settings_model.hpp"
#include "client/audio_settings.hpp"
#include "client/ui/frame_provider_google_video_frame.hpp"
#include "client/video_settings.hpp"
#include "rtc/google/audio_devices.hpp"
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

  void refresh() { devices = enumerator.enumerate(); }

  int rowCount(const QModelIndex &parent = QModelIndex()) const override {
    return devices.size();
  }

  QVariant data(const QModelIndex &index, int role) const override {
    const int row = index.row();
    const std::size_t device_index = static_cast<std::size_t>(row);
    BOOST_ASSERT(device_index < devices.size());
    auto &device = devices[device_index];
    if (role == id_role)
      return QString::fromStdString(device.id);
    BOOST_ASSERT(role == name_role);
    return QString::fromStdString(device.name);
  }

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

  int rowCount(const QModelIndex &parent = QModelIndex()) const override {
    return devices.size();
  }
  QVariant data(const QModelIndex &index, int role) const override {
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

  void refresh() { devices = audio_devices.get_recording_devices(); }

protected:
};
class output_audio_devices_model : public audio_devices_model {
public:
  output_audio_devices_model(rtc::google::audio_devices &audio_devices,
                             client::audio_settings &settings, QObject *parent)
      : audio_devices_model(audio_devices, parent) {
    refresh();
  }

  void refresh() { devices = audio_devices.get_playout_devices(); }

protected:
};
} // namespace

devices_model::devices_model(QObject *parent) : QAbstractListModel(parent) {}

QHash<int, QByteArray> devices_model::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[id_role] = "inputId";
  roles[name_role] = "name";
  return roles;
}

audio_video_settings_model::audio_video_settings_model(
    rtc::google::audio_devices &audio_devices,
    rtc::google::capture::video::enumerator &video_device_enumerator,
    client::audio_settings &audio_settings, video_settings &video_settings_,
    QObject *parent)
    : QObject(parent), audio_settings(audio_settings),
      video_settings_(video_settings_) {
  audio_devices.enumerate();
  output_devices =
      new output_audio_devices_model(audio_devices, audio_settings, this);
  input_devices = new recording_audio_devices_model(audio_devices, this);
  video_devices = new video_devices_model(video_device_enumerator, this);
  audio_output_device_index = audio_settings.get_playout_device();
  audio_input_device_index = audio_settings.get_recording_device();
  update_video_preview();
  video_settings_.on_video_source_changed.connect(
      [this] { update_video_preview(); });
}

audio_video_settings_model::~audio_video_settings_model() = default;

void audio_video_settings_model::onAudioInputDeviceActivated(int index) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", index:" << index;
  audio_settings.set_recording_device(index);
}

void audio_video_settings_model::onAudioOutputDeviceActivated(int index) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", index:" << index;
  audio_settings.set_output_device(index);
}

void audio_video_settings_model::onVideoDeviceActivated(int index) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", index:" << index;
  const auto id =
      static_cast<video_devices_model *>(video_devices)->get_id_by_index(index);
  try {
    video_settings_.change_to_device(id);
  } catch (const boost::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "could not change video device";
    BOOST_ASSERT(false); // TODO implement
  }
}

void audio_video_settings_model::update_video_preview() {
  if (video_preview) {
    video_preview->deleteLater();
    video_preview = nullptr;
  }
  auto source = video_settings_.get_video_source();
  if (source == nullptr)
    return;
  video_preview = new ui::frame_provider_google_video_source(this);
  video_preview->set_source(source);
  video_preview_changed(video_preview);
}
