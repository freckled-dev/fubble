#ifndef UUID_54B433A9_B9B2_4525_B1C8_8BFF1B6D58C1
#define UUID_54B433A9_B9B2_4525_B1C8_8BFF1B6D58C1

#include "client/logger.hpp"
#include <QObject>

namespace client {
class error_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString text MEMBER text NOTIFY text_changed)
  Q_PROPERTY(QString technical MEMBER technical NOTIFY technical_changed)
public:
  error_model(QObject *parent = nullptr);
  enum class type { could_not_connect_to_backend };
  void set_error(const type type_, const std::string technical_);

signals:
  void error();
  void text_changed();
  void technical_changed();

protected:
  QString type_to_string(const type type_) const;
  QString text;
  QString technical;
};
} // namespace client

#endif
