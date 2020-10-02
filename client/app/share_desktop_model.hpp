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
  share_desktop_previews_model(QObject *parent) : QAbstractListModel(parent) {}

  void add(const desktop_sharing_previews::preview &preview) {
    auto &capturer = preview.capturer->get_capturer();
    auto result = new ui::frame_provider_google_video_source(this);
    auto video_source = capturer.get_source();
    BOOST_ASSERT(video_source);
    result->set_source(video_source);
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    players.push_back(result);
    capturers.push_back(preview.capturer);
    endInsertRows();
  }

  void remove(const desktop_sharing_previews::preview &preview) {
    auto found =
        std::find(capturers.cbegin(), capturers.cend(), preview.capturer);
    BOOST_ASSERT(found != capturers.cend());
    if (found == capturers.cend())
      return;
    const int index = std::distance(capturers.cbegin(), found);
    beginRemoveRows(QModelIndex(), index, index);
    capturers.erase(found);
    players.erase(std::next(players.cbegin(), index));
    endRemoveRows();
  }

protected:
  int rowCount([[maybe_unused]] const QModelIndex &index =
                   QModelIndex()) const override {
    return players.size();
  }

  QVariant data(const QModelIndex &index, int role) const override {
    if (role == description_role || role == id_role) {
      auto &capturer = capturers[index.row()]->get_capturer();
      if (role == id_role) {
        const auto id = capturer.get_id();
        return QVariant::fromValue(id);
      }
      if (role == description_role) {
        const std::string title = capturer.get_title();
        return QVariant::fromValue(QString::fromStdString(title));
      }
      BOOST_ASSERT(false);
    }
    return QVariant::fromValue(players[index.row()]);
  }

  enum roles { id_role = Qt::UserRole + 1, description_role, player_role };

  QHash<int, QByteArray> roleNames() const override {
    QHash<int, QByteArray> roles;
    roles[id_role] = "id";
    roles[description_role] = "description";
    roles[player_role] = "player";
    return roles;
  }

  std::vector<std::shared_ptr<rtc::google::capture::desktop::interval_capturer>>
      capturers;
  std::vector<ui::frame_provider_google_video_source *> players;
};
class share_desktop_categories_model : public QAbstractListModel {
  Q_OBJECT
public:
  share_desktop_categories_model(
      const std::shared_ptr<desktop_sharing_previews> &previews_,
      QObject *parent)
      : QAbstractListModel(parent), desktop_previews{previews_} {
    previews[0] = new share_desktop_previews_model(this);
    desktop_previews->on_screen_added.connect(
        [this](auto added) { previews[0]->add(added); });
    desktop_previews->on_screen_removed.connect(
        [this](auto removed) { previews[0]->remove(removed); });
    previews[1] = new share_desktop_previews_model(this);
    desktop_previews->on_window_added.connect(
        [this](auto added) { previews[1]->add(added); });
    desktop_previews->on_window_removed.connect(
        [this](auto removed) { previews[1]->remove(removed); });
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
  std::shared_ptr<desktop_sharing_previews> desktop_previews;
};
class share_desktop_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(share_desktop_categories_model *categories MEMBER categories NOTIFY
                 categories_changed)

public:
  share_desktop_model(
      const std::shared_ptr<client::desktop_sharing> &desktop_sharing_,
      const std::shared_ptr<client::desktop_sharing_previews>
          desktop_sharing_previews_);
  ~share_desktop_model();

  Q_INVOKABLE void startPreviews();
  Q_INVOKABLE void stopPreviews();
  Q_INVOKABLE void shareDesktop(qint64 id);
  Q_INVOKABLE void stopShareDesktop();

signals:
  void categories_changed(share_desktop_categories_model *);

protected:
  client::logger logger{"share_desktop_model"};
  std::shared_ptr<desktop_sharing> desktop_sharing_;
  std::shared_ptr<desktop_sharing_previews> desktop_sharing_previews_;
  share_desktop_categories_model *categories{};
};
} // namespace client

#endif
