#ifndef UUID_1A7B513A_FA47_48C2_950C_01771F50FEAC
#define UUID_1A7B513A_FA47_48C2_950C_01771F50FEAC

#include "client/desktop_sharing.hpp"
#include "client/logger.hpp"
#include "client/ui/frame_provider_google_video_frame.hpp"
#include <QAbstractItemModel>

namespace client {
class share_desktop_preview_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString description MEMBER description NOTIFY description_changed)
  Q_PROPERTY(ui::frame_provider_google_video_source *player MEMBER source NOTIFY
                 source_changed)

public:
  share_desktop_preview_model(desktop_sharing::preview &preview,
                              QObject *parent);
signals:
  void description_changed(QString);
  void source_changed(ui::frame_provider_google_video_source *);

protected:
  QString description;
  ui::frame_provider_google_video_source *source{};
};
class share_desktop_previews_model : public QAbstractListModel {
  Q_OBJECT
public:
  share_desktop_previews_model(desktop_sharing::previews &previews,
                               QObject *parent);
};
class share_desktop_category_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name MEMBER name NOTIFY name_changed)
  Q_PROPERTY(share_desktop_previews_model *previews MEMBER previews NOTIFY
                 previews_changed)
public:
  share_desktop_category_model(QString title, QObject *parent);

signals:
  void name_changed(QString);
  void previews_changed(share_desktop_previews_model *);

protected:
  QString name;
  share_desktop_previews_model *previews{};
};
class share_desktop_categories_model : public QAbstractListModel {
  Q_OBJECT
public:
  share_desktop_categories_model(desktop_sharing::previews &screens,
                                 desktop_sharing::previews &windows,
                                 QObject *parent);

protected:
  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
};
class share_desktop_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(share_desktop_categories_model *categories MEMBER categories NOTIFY
                 categories_changed)

public:
  share_desktop_model(
      const std::shared_ptr<client::desktop_sharing> &desktop_sharing_);
  ~share_desktop_model();

  Q_INVOKABLE void startPreviews();
  Q_INVOKABLE void stopPreviews();
  // TODO
  // Q_INVOKABLE void shareDesktop();

signals:
  void categories_changed(share_desktop_categories_model *);

protected:
  client::logger logger{"share_desktop_model"};
  std::shared_ptr<desktop_sharing> desktop_sharing_;
  share_desktop_categories_model *categories{};
  desktop_sharing::previews screens;
  desktop_sharing::previews windows;
};
} // namespace client

#endif
