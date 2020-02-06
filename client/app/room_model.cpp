#include "room_model.hpp"
#include "room.hpp"
#include "session/room.hpp"
#include "ui/thread_helper.hpp"

using namespace client;

room_model::room_model(boost::executor &backend_thread,
                       const std::shared_ptr<room> &room_, QObject *parent)
    : QObject(parent), backend_thread(backend_thread), room_(room_) {
  participants = new participants_model(*room_, this);
  get_name();
}

void room_model::get_name() {
  backend_thread.submit([this] {
    auto name_ = room_->get_name();
    post_to_object([this, name_] {
      BOOST_LOG_SEV(logger, logging::severity::trace)
          << "setting room name to:" << name_;
      auto nameCasted = QString::fromStdString(name_);
      name = nameCasted;
      name_changed(nameCasted);
    });
  });
}
