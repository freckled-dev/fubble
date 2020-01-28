// TODO include guard

#include <QObject>

namespace client {

class participant_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name MEMBER name NOTIFY name_changed)
public:
  participant_model(QObject *parent);

signals:
  void name_changed(QString);

protected:
  QString name;
};

} // namespace client
