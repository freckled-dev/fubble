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
    [[maybe_unused]] auto got_room = room_.get();
    BOOST_ASSERT(got_room);
    joined();
    return;
  } catch (const joiner::update_required &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "update required, error:" << boost::diagnostic_information(error);
    auto minimum_version = QString::fromStdString(
        *boost::get_error_info<joiner::minimum_version_info>(error));
    error_model_.set_error(
        tr("The version you are using to connect is outdated. The minimum "
           "required version is '%1'. Please visit <a "
           "href=\"https://fubble.io\">https://fubble.io</a> to "
           "get the newest version.")
            .arg(minimum_version),
        error);
  } catch (const std::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::warning)
        << "could not join room, what:" << error.what();
    error_model_.set_error(error_model::type::could_not_connect_to_backend,
                           error.what());
  } catch (...) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << __FUNCTION__ << ", catch(...)";
    BOOST_ASSERT(false);
  }
  join_failed();
}
