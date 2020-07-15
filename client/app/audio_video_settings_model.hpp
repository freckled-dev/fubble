#ifndef UUID_C9B76A9B_14FF_41A0_A176_747182459D11
#define UUID_C9B76A9B_14FF_41A0_A176_747182459D11

#include "client/logger.hpp"
#include <QAbstractItemModel>

namespace rtc::google::capture::video {
class enumerator;
}
namespace rtc::google {
class audio_devices;
}
namespace client {
class audio_settings;
class video_settings;
namespace ui {
class frame_provider_google_video_source;
}
class devices_model : public QAbstractListModel {
  Q_OBJECT
public:
  enum roles { id_role = Qt::UserRole + 1, name_role };
  devices_model(QObject *parent);

protected:
  QHash<int, QByteArray> roleNames() const override;
};
class audio_video_settings_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(client::ui::frame_provider_google_video_source *videoPreview MEMBER
                 video_preview NOTIFY video_preview_changed)
  Q_PROPERTY(int userAudioInputDeviceIndex MEMBER audio_input_device_index
                 NOTIFY audio_input_device_index_changed)
  Q_PROPERTY(int userAudioOutputDeviceIndex MEMBER audio_output_device_index
                 NOTIFY audio_output_device_index_changed)
  Q_PROPERTY(int userVideoDeviceIndex MEMBER video_device_index NOTIFY
                 video_device_index_changed)
  Q_PROPERTY(client::devices_model *inputDevices MEMBER input_devices NOTIFY
                 input_devices_changed)
  Q_PROPERTY(client::devices_model *outputDevices MEMBER output_devices NOTIFY
                 output_devices_changed)
  Q_PROPERTY(client::devices_model *videoDevices MEMBER video_devices NOTIFY
                 video_devices_changed)

public:
  audio_video_settings_model(
      rtc::google::audio_devices &audio_devices,
      rtc::google::capture::video::enumerator &video_device_enumerator,
      audio_settings &audio_settings_, video_settings &video_settings_,
      QObject *parent = nullptr);
  ~audio_video_settings_model();

  Q_INVOKABLE void onAudioInputDeviceActivated(int index);
  Q_INVOKABLE void onAudioOutputDeviceActivated(int index);
  Q_INVOKABLE void onVideoDeviceActivated(int index);

signals:
  void video_preview_changed(ui::frame_provider_google_video_source *);
  void audio_input_device_index_changed(int);
  void audio_output_device_index_changed(int);
  void video_device_index_changed(int);
  void input_devices_changed(client::devices_model *);
  void output_devices_changed(client::devices_model *);
  void video_devices_changed(client::devices_model *);

protected:
  void update_video_preview();

  client::logger logger{"audio_video_settings_model"};
  client::audio_settings &audio_settings;
  video_settings &video_settings_;
  ui::frame_provider_google_video_source *video_preview{};
  int audio_input_device_index{};
  int audio_output_device_index{};
  int video_device_index{};
  client::devices_model *input_devices{};
  client::devices_model *output_devices{};
  client::devices_model *video_devices{};
};
} // namespace client

#endif
