#ifndef UUID_3F20FC60_F52B_4FFE_9CFA_8134CAE97A86
#define UUID_3F20FC60_F52B_4FFE_9CFA_8134CAE97A86

#include "logging/logger.hpp"
#include "participant_model.hpp"
#include "room.hpp"
#include <QAbstractItemModel>

namespace client {
class participant;
class participants_model : public QAbstractListModel {
  Q_OBJECT
public:
  participants_model(room &room_, QObject *parent);

  enum participant_roles { participant_role = Qt::UserRole + 1 };

  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;

protected:
  QHash<int, QByteArray> roleNames() const override;
  void on_joins(const std::vector<participant *> &joins);
  void on_leaves(std::vector<std::string> leaves);

  mutable logging::logger logger;
  room &room_;
  std::vector<participant_model *> participants;
  std::vector<boost::signals2::scoped_connection> signal_connections;
};

} // namespace client

#endif
