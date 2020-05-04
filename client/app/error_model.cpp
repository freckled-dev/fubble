#include "error_model.hpp"

using namespace client;

error_model::error_model(QObject *parent) : QObject(parent) {}

void error_model::set_error(const type type_, const std::string technical_) {
  technical = QString::fromStdString(technical_);
  technical_changed();
  text = type_to_string(type_);
  text_changed();
  error();
}

QString error_model::type_to_string(const type type_) const {
  switch (type_) {
  case type::could_not_connect_to_backend:
    return tr("Could not connect to backend services. Please verify that you "
              "can access https://fubble.io");
  default:
    BOOST_ASSERT(false);
  }
  return "";
}
