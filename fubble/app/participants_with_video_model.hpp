#ifndef UUID_FC8EB317_730A_47D7_999E_DFF57083EA87
#define UUID_FC8EB317_730A_47D7_999E_DFF57083EA87

#include "fubble/client/logger.hpp"
#include "participants_model.hpp"
#include <QAbstractListModel>
#include <deque>

namespace client {
class participants_with_video_model : public QAbstractListModel {
  Q_OBJECT
public:
  participants_with_video_model(participants_model &participants);

  enum participant_roles { participant_role = Qt::UserRole + 1 };

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;

protected slots:
  void on_participants_inserted(const QModelIndex &parent, int first, int last);
  void on_participants_about_to_be_removed(const QModelIndex &parent, int first,
                                           int last);

protected:
  void add_participant(participant_model &add);
  void remove_participant(participant_model &remove);
  void remove_participant(const std::string &id);
  QHash<int, QByteArray> roleNames() const override;
  using participants_container_type = std::deque<participant_model *>;
  participants_container_type::iterator find(const std::string &id);
  participant_model &get_participant_from_participants_by_index(int index);

  client::logger logger{"participants_with_video_model"};
  participants_model &participants;
  participants_container_type participants_container;
};
} // namespace client

#endif
