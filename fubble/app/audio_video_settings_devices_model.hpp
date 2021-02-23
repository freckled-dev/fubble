#ifndef UUID_2C19AB1C_E61F_4B57_A0FA_7E028A3C62F3
#define UUID_2C19AB1C_E61F_4B57_A0FA_7E028A3C62F3

#include "fubble/client/audio_device_settings.hpp"
#include "fubble/rtc/audio_devices.hpp"
#include "fubble/rtc/google/capture/video/device.hpp"
#include "fubble/rtc/google/capture/video/enumerator.hpp"
#include <QAbstractItemModel>

namespace client {
class devices_model : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(bool available MEMBER available NOTIFY available_changed)
public:
  enum roles { id_role = Qt::UserRole + 1, name_role };
  devices_model(QObject *parent);
  virtual void refresh() = 0;

signals:
  void available_changed(bool);

protected:
  void update_available(const bool available_);

  QHash<int, QByteArray> roleNames() const override;
  bool available{};
};
class video_devices_model : public devices_model {
  Q_OBJECT
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
  Q_OBJECT
public:
  audio_devices_model(std::shared_ptr<rtc::audio_devices> audio_devices,
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

  std::shared_ptr<rtc::audio_devices> audio_devices;
  rtc::audio_devices::devices devices;
};
class recording_audio_devices_model : public audio_devices_model {
public:
  recording_audio_devices_model(
      std::shared_ptr<rtc::audio_devices> audio_devices, QObject *parent)
      : audio_devices_model(audio_devices, parent) {}

  void refresh() override {
    auto new_devices = audio_devices->get_recording_devices();
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
  Q_OBJECT
public:
  output_audio_devices_model(std::shared_ptr<rtc::audio_devices> audio_devices,
                             client::audio_device_settings &, QObject *parent)
      : audio_devices_model(audio_devices, parent) {}

  void refresh() override {
    auto new_devices = audio_devices->get_playout_devices();
    if (new_devices == devices)
      return;
    beginResetModel();
    devices = new_devices;
    endResetModel();
    update_available(true);
  }

protected:
};

} // namespace client

#endif
