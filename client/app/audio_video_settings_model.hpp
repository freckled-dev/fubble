#ifndef UUID_C9B76A9B_14FF_41A0_A176_747182459D11
#define UUID_C9B76A9B_14FF_41A0_A176_747182459D11

#include "client/logger.hpp"
#include <QAbstractItemModel>

namespace client {
namespace ui {
class frame_provider_google_video_source;
}
class devices_model : public QAbstractListModel {
  Q_OBJECT
public:
  enum roles { id_role = Qt::UserRole + 1, name_role };
  devices_model(QObject *parent);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;

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
  audio_video_settings_model(QObject *parent = nullptr);
  ~audio_video_settings_model();

signals:
  void video_preview_changed(ui::frame_provider_google_video_source *);
  void audio_input_device_index_changed(int);
  void audio_output_device_index_changed(int);
  void video_device_index_changed(int);
  void input_devices_changed(client::devices_model *);
  void output_devices_changed(client::devices_model *);
  void video_devices_changed(client::devices_model *);

protected:
  client::logger logger{"audio_video_settings_model"};
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
