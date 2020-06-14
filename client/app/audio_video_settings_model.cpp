#include "audio_video_settings_model.hpp"

using namespace client;

devices_model::devices_model(QObject *parent) : QAbstractListModel(parent) {}

int devices_model::rowCount(const QModelIndex &parent) const {
  // TODO
  return 0;
}

QVariant devices_model::data(const QModelIndex &index, int role) const {
  (void)index;
  (void)role;
  return QVariant();
}

QHash<int, QByteArray> devices_model::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[id_role] = "inputId";
  roles[name_role] = "name";
  return roles;
}

audio_video_settings_model::audio_video_settings_model(QObject *parent)
    : QObject(parent) {
  input_devices = new devices_model(this);
  output_devices = new devices_model(this);
  video_devices = new devices_model(this);
}

audio_video_settings_model::~audio_video_settings_model() = default;

