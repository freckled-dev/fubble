#ifndef UUID_D978AFC1_99B5_46E6_8830_CA90360352F5
#define UUID_D978AFC1_99B5_46E6_8830_CA90360352F5

#include "client/logger.hpp"
#include <QObject>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace client {
class leaver;
class leave_model : public QObject {
  Q_OBJECT
public:
  enum class reason : int { normal = 0 };

  leave_model(leaver &leaver_, QObject *parent = nullptr);

  Q_INVOKABLE void leave();

signals:
  void left(int reason);

protected:
  void on_left(boost::future<void> &result);

  client::logger logger{"leave_model"};
  leaver &leaver_;
  boost::inline_executor executor;
};
} // namespace client

#endif
