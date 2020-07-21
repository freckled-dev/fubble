#ifndef UUID_5D0F470B_C872_46EB_8804_1972618A74E6
#define UUID_5D0F470B_C872_46EB_8804_1972618A74E6

#include "client/logger.hpp"
#include <QObject>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace client {
class joiner;
class room_model;
class room;
class error_model;
class model_creator;
class join_model : public QObject {
  Q_OBJECT
public:
  join_model(model_creator &model_factory, error_model &error_model_,
             joiner &joiner_);
  ~join_model();

  Q_INVOKABLE void join(const QString &room, const QString &name);

signals:
  void joined(client::room_model *room);
  void join_failed();

protected:
  void on_joined(boost::future<std::shared_ptr<room>> room_);

  client::logger logger{"join_model"};
  model_creator &model_factory;
  error_model &error_model_;
  joiner &joiner_;
  boost::inline_executor qt_executor;
};
} // namespace client

// Q_DECLARE_METATYPE(client::join_model *)

#endif
