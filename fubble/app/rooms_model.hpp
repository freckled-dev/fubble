#ifndef UUID_5FE653B5_2E7D_42EC_AD07_243EFF0B0EF2
#define UUID_5FE653B5_2E7D_42EC_AD07_243EFF0B0EF2

#include <QObject>
#include <boost/signals2/signal.hpp>
#include <memory>
#include <vector>

namespace client {
class model_creator;
class room;
class room_model;
class rooms;
class rooms_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(client::room_model *room MEMBER room_ NOTIFY room_changed)
public:
  rooms_model(std::shared_ptr<rooms> rooms_,
              std::shared_ptr<model_creator> model_creator_,
              QObject *parent = nullptr);

signals:
  void room_changed(room_model *);

protected:
  void on_room_set();

  std::shared_ptr<rooms> rooms_;
  std::shared_ptr<model_creator> model_creator_;
  room_model *room_{};

  std::vector<boost::signals2::scoped_connection> signal_connections;
};
} // namespace client

#endif
