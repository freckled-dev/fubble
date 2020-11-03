#include "language_settings_model.hpp"
#include <QCoreApplication>
#include <QDirIterator>
#include <QQmlEngine>
#include <QTranslator>
#include <boost/assert.hpp>

using namespace client;

languages_model::languages_model(QObject *parent)
    : QAbstractListModel(parent) {}

int languages_model::rowCount(const QModelIndex &) const { return 2; }

QVariant languages_model::data(const QModelIndex &index, int role) const {
  std::array<QString, 2> results = {QString(tr("English")),
                                    QString(tr("Deutsch"))};
  BOOST_ASSERT(role == description_role);
  return results[index.row()];
}

language_settings_model::language_settings_model(QQmlEngine &engine,
                                                 QObject *parent)
    : QObject(parent), languages{new languages_model(this)}, engine(engine) {
  set_selected(1);
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
  remove_translator();
  if (change == 1) {
    translator = std::make_unique<QTranslator>();
    // this ishacky. it's not a ts file, but a qml file.
    // https://github.com/mesonbuild/meson/issues/7925
    [[maybe_unused]] bool loaded = translator->load(":/i18n/de.ts");
    BOOST_ASSERT(loaded);
    [[maybe_unused]] bool installed =
        QCoreApplication::installTranslator(translator.get());
    BOOST_ASSERT(installed);
  }
  engine.retranslate();
}

void language_settings_model::remove_translator() {
  if (!translator)
    return;
  QCoreApplication::removeTranslator(translator.get());
  translator.reset();
}
