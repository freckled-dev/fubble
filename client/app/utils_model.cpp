#include "utils_model.hpp"
#include <QApplication>
#include <QClipboard>

using namespace client;

utils_model::utils_model(QObject *parent)
    : QObject(parent) {}

utils_model::~utils_model() {}

void utils_model::copyToClipboard(const QString text) {
  BOOST_LOG_SEV(logger, logging::severity::trace)
        << "copy to clipboard, text: " << text.toStdString();
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(text);
}



