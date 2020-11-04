#include "language_settings_model.hpp"
#include <QCoreApplication>
#include <QDirIterator>
#include <QQmlEngine>
#include <QSettings>
#include <QTranslator>
#include <boost/assert.hpp>

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
  QSettings settings;
  if (!settings.contains("language"))
    return;
  QString set = settings.value("language").toString();
  if (set == "en")
    selected = 1;
  if (set == "de")
    selected = 2;
  on_selected_changed(selected);
  set_language(set);
}

language_settings_model::~language_settings_model() = default;

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
  if (change == 1)
    setting = "en";
  if (change == 2)
    setting = "de";
  set_language(setting);
}

void language_settings_model::set_language(QString setting) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", uiLanguage:" << engine.uiLanguage().toStdString()
      << ", chaning to " << setting.toStdString();
  engine.setUiLanguage(setting);
  QSettings settings;
  if (setting.isEmpty()) {
    settings.remove("language");
    return;
  }
  settings.setValue("language", setting);
}
