#ifndef UUID_1A7B513A_FA47_48C2_950C_01771F50FEAC
#define UUID_1A7B513A_FA47_48C2_950C_01771F50FEAC

#include "client/logger.hpp"
#include <QObject>

namespace client {
class share_desktop_model : public QObject {
  Q_OBJECT
  // TODO Q_PROPERTY(... categories ...)

public:
  share_desktop_model();
  ~share_desktop_model();

  Q_INVOKABLE void startPreviews();
  Q_INVOKABLE void stopPreviews();
  // TODO
  // Q_INVOKABLE void shareDesktop();

protected:
  client::logger logger{"share_desktop_model"};
};
} // namespace client

#endif
