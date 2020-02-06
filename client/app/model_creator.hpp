#ifndef UUID_7C6181D8_1BDC_4398_AC8C_618BA9F2CC5C
#define UUID_7C6181D8_1BDC_4398_AC8C_618BA9F2CC5C

#include <QObject>
#include <boost/thread/executor.hpp>

namespace client {
class room_model;
class room;
class model_creator {
public:
  model_creator(boost::executor &backend_thread);
  room_model *create_room_model(const std::shared_ptr<room> &room_,
                                QObject *parent);

private:
  boost::executor &backend_thread;
};
} // namespace client

#endif
