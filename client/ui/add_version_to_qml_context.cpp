#include "add_version_to_qml_context.hpp"
#include "utils/version.hpp"
#include <QQmlContext>

using namespace client::ui;

add_version_to_qml_context::add_version_to_qml_context(QQmlContext &context) {
  QString version = QString::fromStdString(utils::version());
  context.setContextProperty("appVersion", version);
}
