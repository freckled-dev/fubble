#include "error_model.hpp"
#include <boost/exception/all.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

using namespace client;

error_model::error_model(QObject *parent) : QObject(parent) {}

void error_model::set_error(const type type_, const std::string technical_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << fmt::format("{}, type:{}, technical:{}", __FUNCTION__,
                     static_cast<int>(type_), technical_);
  technical = QString::fromStdString(technical_);
  technical_changed();
  text = type_to_string(type_);
  text_changed();
  error();
}

void error_model::set_error(const type type_, const boost::exception &error) {
  const std::string message = boost::diagnostic_information(error);
  set_error(type_, message);
}

QString error_model::type_to_string(const type type_) const {
  switch (type_) {
  case type::could_not_connect_to_backend:
    return tr("Could not connect to backend services. Please verify that you "
              "can access <a href='https://fubble.io'>https://fubble.io</a>");
  case type::failed_to_start_camera:
    return tr("Could not initialize camera.");
  default:
    BOOST_ASSERT(false);
  }
  return "";
}

void error_model::set_error(QString description,
                            const boost::exception &exception_) {
  const std::string technical_ = boost::diagnostic_information(exception_);
  technical = QString::fromStdString(technical_);
  technical_changed();
  text = description;
  text_changed();
  error();
}
