#ifndef UUID_912B7A62_19DE_45A9_B92E_0FC0046CE8C5
#define UUID_912B7A62_19DE_45A9_B92E_0FC0046CE8C5

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

// Q_DECLARE_METATYPE(client::participant_model *)

#endif
