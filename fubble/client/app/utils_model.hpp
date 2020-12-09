#ifndef UUID_5D0F470B_C872_46EB_8804_1982618A74E7
#define UUID_5D0F470B_C872_46EB_8804_1982618A74E7

#include "fubble/client/logger.hpp"
#include <QObject>

namespace client {

class utils_model : public QObject {
  Q_OBJECT

public:
  utils_model(QObject *parent = nullptr);
  ~utils_model();

  Q_INVOKABLE void copyToClipboard(const QString text);

protected:
  client::logger logger{"utils_model"};
};
} // namespace client

#endif
