#ifndef UUID_1A7B513A_FA47_48C2_950C_01771F50FEAC
#define UUID_1A7B513A_FA47_48C2_950C_01771F50FEAC

#include "client/desktop_sharing.hpp"
#include "client/logger.hpp"
#include <QAbstractItemModel>

namespace client {
class share_desktop_preview_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString description MEMBER description NOTIFY description_changed)

public:
  share_desktop_preview_model(desktop_sharing::preview &preview,
                              QObject *parent);
signals:
  void description_changed(QString);

protected:
  QString description;
};
class share_desktop_previews_model : public QAbstractListModel {
  Q_OBJECT
public:
  share_desktop_previews_model(desktop_sharing::previews &previews,
                               QObject *parent);
};
class share_desktop_category_model : public QObject {
  Q_OBJECT
public:
  share_desktop_category_model(QString title, QObject *parent);
#if 0
  property string name: qsTr("Screens")
  property list<QtObject> previews: [
#endif
};
class share_desktop_categories_model : public QAbstractListModel {
  Q_OBJECT
public:
  share_desktop_categories_model(desktop_sharing::previews &screens,
                                 desktop_sharing::previews &windows,
                                 QObject *parent);
};
class share_desktop_model : public QObject {
  Q_OBJECT
  // TODO Q_PROPERTY(... categories ...)

public:
  share_desktop_model(

      const std::shared_ptr<client::desktop_sharing> &desktop_sharing_);
  ~share_desktop_model();

  Q_INVOKABLE void startPreviews();
  Q_INVOKABLE void stopPreviews();
  // TODO
  // Q_INVOKABLE void shareDesktop();

protected:
  client::logger logger{"share_desktop_model"};
  std::shared_ptr<desktop_sharing> desktop_sharing_;
  desktop_sharing::previews screens;
  desktop_sharing::previews windows;
};
} // namespace client

#endif
