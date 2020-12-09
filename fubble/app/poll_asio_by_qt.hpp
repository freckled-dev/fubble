#ifndef UUID_E11ECF09_4A1B_46A3_89E2_00CBC77113C0
#define UUID_E11ECF09_4A1B_46A3_89E2_00CBC77113C0

#include <QtCore/QTimer>
#include <boost/asio/io_context.hpp>

namespace client {
class poll_asio_by_qt : public QObject {
  Q_OBJECT
public:
  poll_asio_by_qt(boost::asio::io_context &context);
  void run();

protected slots:
  void on_timeout();

protected:
  boost::asio::io_context &context;
  QTimer timer;
};
} // namespace client

#endif
