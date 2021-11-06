#ifndef UUID_3F20FC60_F52B_4FFE_9CFA_8134CAE97A86
#define UUID_3F20FC60_F52B_4FFE_9CFA_8134CAE97A86

#include "fubble/client/logger.hpp"
#include "fubble/client/room.hpp"
#include "participant_model.hpp"
#include <QAbstractItemModel>
#include <deque>

namespace client {
class participant;
class model_creator;
class participants_model : public QAbstractListModel {
  Q_OBJECT
public:
  participants_model(model_creator &model_creator_, room &room_,
                     QObject *parent);

  enum participant_roles { participant_role = Qt::UserRole + 1 };

  boost::optional<participant_model *> get_own() const;
  sigslot::signal<> on_own_changed;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;

protected:
  QHash<int, QByteArray> roleNames() const override;
  void on_joins(const std::vector<participant *> &joins);
  void on_leaves(std::vector<std::string> leaves);

  virtual std::vector<participant *>
  filter_joining(const std::vector<participant *> &joining);
  std::vector<participant *>
  filter_out_bots(const std::vector<participant *> &joining);

  mutable client::logger logger{"participants_model"};
  model_creator &model_creator_;
  room &room_;
  std::deque<participant_model *> participants;
  std::vector<sigslot::scoped_connection> signal_connections;
};

} // namespace client

#endif
