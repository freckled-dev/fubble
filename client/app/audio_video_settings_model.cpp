#include "audio_video_settings_model.hpp"
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
    return devices.size() + 1; // +1 for "none"
  }
  QVariant data(const QModelIndex &index, int role) const override {
    const int row = index.row();
    if (row == 0) {
      if (role == id_role)
        return QVariant{};
      return tr("None");
    }
    const std::size_t device_index = static_cast<std::size_t>(row - 1);
    BOOST_ASSERT(device_index < devices.size());
    auto &device = devices[device_index];
    if (role == id_role)
      return QString::fromStdString(device.id);
    BOOST_ASSERT(role == name_role);
    return QString::fromStdString(device.name);
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
                             QObject *parent)
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
    QObject *parent)
    : QObject(parent) {
  audio_devices.enumerate();
  input_devices = new output_audio_devices_model(audio_devices, this);
  output_devices = new recording_audio_devices_model(audio_devices, this);
  video_devices = new video_devices_model(video_device_enumerator, this);
  // TODO select the correct one!
  // TODO handle change
  // TODO show video preview
}

audio_video_settings_model::~audio_video_settings_model() = default;
