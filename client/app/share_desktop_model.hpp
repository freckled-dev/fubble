#ifndef UUID_1A7B513A_FA47_48C2_950C_01771F50FEAC
#define UUID_1A7B513A_FA47_48C2_950C_01771F50FEAC

#include "client/desktop_sharing.hpp"
#include "client/logger.hpp"
#include "client/ui/frame_provider_google_video_frame.hpp"
#include <QAbstractItemModel>

namespace client {
class share_desktop_previews_model : public QAbstractListModel {
  Q_OBJECT
public:
  share_desktop_previews_model(desktop_sharing::previews &previews,
                               QObject *parent)
      : QAbstractListModel(parent), previews(previews) {
    std::transform(
        previews.begin(), previews.end(), std::back_inserter(players),
        [&](desktop_sharing::preview &preview) {
          auto &capturer = preview.capturer->get_capturer();
          auto result = new ui::frame_provider_google_video_source(this);
          result->set_source(&capturer);
          preview.capturer->start(); // TODO returns a future!
          return result;
        });
  }

protected:
  int rowCount(const QModelIndex &) const override { return previews.size(); }

  QVariant data(const QModelIndex &index, int role) const override {
    if (role == description_role) {
      const std::string title =
          previews[index.row()].capturer->get_capturer().get_title();
      return QVariant::fromValue(QString::fromStdString(title));
    }
    return QVariant::fromValue(players[index.row()]);
  }

  enum roles { description_role = Qt::UserRole + 1, player_role };

  QHash<int, QByteArray> roleNames() const override {
    QHash<int, QByteArray> roles;
    roles[description_role] = "description";
    roles[player_role] = "player";
    return roles;
  }

  desktop_sharing::previews &previews;
  std::vector<ui::frame_provider_google_video_source *> players;
};
class share_desktop_categories_model : public QAbstractListModel {
  Q_OBJECT
public:
  share_desktop_categories_model(desktop_sharing::previews &screens,
                                 desktop_sharing::previews &windows,
                                 QObject *parent)
      : QAbstractListModel(parent) {
    previews[0] = new share_desktop_previews_model(screens, this);
    previews[1] = new share_desktop_previews_model(windows, this);
  }

protected:
  int rowCount(const QModelIndex &) const override { return 2; }

  QVariant data(const QModelIndex &index, int role) const override {
    if (role == name_role) {
      if (index.row() == 0)
        return QVariant::fromValue(tr("Screens"));
      return QVariant::fromValue(tr("Windows"));
    }
    return QVariant::fromValue(previews[index.row()]);
  }

  enum roles { name_role = Qt::UserRole + 1, previews_role };

  QHash<int, QByteArray> roleNames() const override {
    QHash<int, QByteArray> roles;
    roles[name_role] = "name";
    roles[previews_role] = "previews";
    return roles;
  }

  std::array<share_desktop_previews_model *, 2> previews;
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
