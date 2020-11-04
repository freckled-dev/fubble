#include "language_settings_model.hpp"
#include <QCoreApplication>
#include <QDirIterator>
#include <QQmlEngine>
#include <QTranslator>
#include <boost/assert.hpp>
extern "C" {
#include <stdio.h>
}

using namespace client;

languages_model::languages_model(QObject *parent)
    : QAbstractListModel(parent) {}

int languages_model::rowCount(const QModelIndex &) const { return 3; }

QVariant languages_model::data(const QModelIndex &index, int role) const {
  std::array<QString, 3> results;
  results[0] = tr("System Language");
  results[1] = tr("English");
  results[2] = tr("Deutsch");
  BOOST_ASSERT(role == description_role);
  return results[index.row()];
}

language_settings_model::language_settings_model(QQmlEngine &engine,
                                                 QObject *parent)
    : QObject(parent), languages{new languages_model(this)}, engine(engine) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", env LANG:" << getenv("LANG");
}

language_settings_model::~language_settings_model() { remove_translator(); }

void language_settings_model::set_selected(int change) {
  if (change == selected)
    return;
  selected = change;
#if 0
  QDirIterator it(":", QDirIterator::Subdirectories);
  while (it.hasNext()) {
    qDebug() << it.next();
  }
#endif
  QString setting;
  if (change == 2)
    setting = "de";
  if (change == 1)
    setting = "en";
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", uiLanguage:" << engine.uiLanguage().toStdString()
      << ", chaning to " << setting.toStdString();
  engine.setUiLanguage(setting);
}

void language_settings_model::remove_translator() {
  if (!translator)
    return;
  QCoreApplication::removeTranslator(translator.get());
  translator.reset();
}
