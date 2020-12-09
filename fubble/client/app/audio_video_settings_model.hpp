#ifndef UUID_C9B76A9B_14FF_41A0_A176_747182459D11
#define UUID_C9B76A9B_14FF_41A0_A176_747182459D11

#include "audio_video_settings_devices_model.hpp"
#include "client/logger.hpp"
#include "rtc/video_devices.hpp"
#include "utils/timer.hpp"
#include <QAbstractItemModel>

namespace rtc::google::capture::video {
class device;
class device_factory;
} // namespace rtc::google::capture::video
namespace rtc::google {
class audio_devices;
}
namespace client {
namespace ui {
class frame_provider_google_video_device;
}
class error_model;
class video_settings;
class audio_video_settings_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(client::ui::frame_provider_google_video_device *videoPreview READ
                 get_video NOTIFY video_changed)
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
  Q_PROPERTY(bool watchForNewAudioDevices MEMBER enable_update_audio_devices
                 NOTIFY update_audio_devices_changed)

public:
  audio_video_settings_model(
      rtc::google::audio_devices &audio_devices,
      rtc::video_devices &video_device_enumerator,
      rtc::google::capture::video::device_factory &video_device_factory,
      audio_device_settings &audio_settings_, video_settings &video_settings_,
      error_model &error_model_,
      std::shared_ptr<utils::timer_factory> timer_factory,
      QObject *parent = nullptr);
  ~audio_video_settings_model();

  Q_INVOKABLE void onAudioInputDeviceActivated(int index);
  Q_INVOKABLE void onAudioOutputDeviceActivated(int index);
  Q_INVOKABLE void onVideoDeviceActivated(int index);

signals:
  void audio_input_device_index_changed(int);
  void audio_output_device_index_changed(int);
  void video_device_index_changed(int);
  void input_devices_changed(client::devices_model *);
  void output_devices_changed(client::devices_model *);
  void video_devices_changed(client::devices_model *);
  void video_changed(ui::frame_provider_google_video_device *);
  void update_audio_devices_changed(bool);

protected:
  void update_video_device_index();
  ui::frame_provider_google_video_device *get_video();
  void reset_video();
  void update_audio_devices();
  void on_enable_update_audio_devices(bool);

  client::logger logger{"audio_video_settings_model"};
  rtc::google::audio_devices &audio_devices;
  std::shared_ptr<utils::interval_timer> audio_devices_timer;
  rtc::video_devices &video_device_enumerator;
  client::audio_device_settings &audio_settings;
  video_settings &video_settings_;
  rtc::google::capture::video::device_factory &video_device_factory;
  error_model &error_model_;
  std::shared_ptr<utils::timer_factory> timer_factory;
  bool enable_update_audio_devices{};
  int audio_input_device_index{};
  int audio_output_device_index{};
  int video_device_index{};
  client::devices_model *input_devices{};
  client::devices_model *output_devices{};
  client::devices_model *video_devices{};
  std::unique_ptr<rtc::google::capture::video::device> video_device;
  std::unique_ptr<ui::frame_provider_google_video_device> video;
};
} // namespace client

#endif
