#include "join_model.hpp"
#include "client/joiner.hpp"
#include "client/own_media.hpp"
#include "client/room.hpp"
#include "error_model.hpp"
#include "model_creator.hpp"
#include "room_model.hpp"

using namespace client;

join_model::join_model(model_creator &model_factory, error_model &error_model_,
                       joiner &joiner_)
    : model_factory(model_factory), error_model_(error_model_),
      joiner_(joiner_) {}

join_model::~join_model() = default;

void join_model::join(const QString &room, const QString &name) {
  joiner::parameters parameters;
  parameters.room = room.toStdString();
  parameters.name = name.toStdString();
  joiner_.join(parameters).then(qt_executor, [this](auto room) {
    on_joined(std::move(room));
  });
}

void join_model::on_joined(boost::future<std::shared_ptr<class room>> room_) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "on_joined";
  try {
    auto got_room = room_.get();
    auto room_model_ = model_factory.create_room_model(got_room, this);
    joined(room_model_);
  } catch (const std::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "could not join room, what:" << error.what();
    error_model_.set_error(error_model::type::could_not_connect_to_backend,
                           error.what());
    join_failed();
  }
}
