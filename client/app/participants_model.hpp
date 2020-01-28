// TODO include guard!

#include "participant_model.hpp"
#include <QAbstractItemModel>

namespace client {

class participants_model : public QAbstractListModel {
  Q_OBJECT
public:
  participants_model(QObject *parent);

  // TODO continue at
  // https://doc.qt.io/qt-5/qtquick-modelviewsdata-cppmodels.html#qabstractitemmodel-subclass

  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;

protected:
  std::vector<participant_model *> participants;
};

} // namespace client
