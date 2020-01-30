#ifndef UUID_3F20FC60_F52B_4FFE_9CFA_8134CAE97A86
#define UUID_3F20FC60_F52B_4FFE_9CFA_8134CAE97A86

#include "logging/logger.hpp"
#include "participant_model.hpp"
#include "room.hpp"
#include <QAbstractItemModel>

namespace session {
struct participant;
}
namespace client {

class participants_model : public QAbstractListModel {
  Q_OBJECT
public:
  participants_model(room &room_, QObject *parent);

  enum participant_roles { participant_role = Qt::UserRole + 1 };

  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;

protected:
  QHash<int, QByteArray> roleNames() const override;

  mutable logging::logger logger;
  room &room_;
  std::vector<participant_model *> participants;
};

} // namespace client

#endif
