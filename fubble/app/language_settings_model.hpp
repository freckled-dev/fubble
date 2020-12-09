#ifndef UUID_8133A390_B8DF_40EF_BE53_2F723267CAFE
#define UUID_8133A390_B8DF_40EF_BE53_2F723267CAFE

#include "fubble/client/logger.hpp"
#include <QAbstractItemModel>
#include <memory>

class QTranslator;
class QQmlEngine;

namespace client {

class languages_model : public QAbstractListModel {
  Q_OBJECT
public:
  languages_model(QObject *parent);

protected:
  enum roles { language_id_role = Qt::UserRole + 1, description_role };
  int rowCount(const QModelIndex &) const override;

  QVariant data(const QModelIndex &index, int role) const override;

  QHash<int, QByteArray> roleNames() const override {
    QHash<int, QByteArray> roles;
    roles[language_id_role] = "languageId";
    roles[description_role] = "description";
    return roles;
  }
};

class language_settings_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(int languageIndex MEMBER selected WRITE set_selected NOTIFY
                 on_selected_changed)
  Q_PROPERTY(
      languages_model *languages MEMBER languages NOTIFY on_languages_changed)
public:
  language_settings_model(QQmlEngine &engine, QObject *parent = nullptr);
  ~language_settings_model();

signals:
  void on_selected_changed(int);
  void on_languages_changed(languages_model *);

protected:
  void set_selected(int);
  void set_language(QString setting);

  client::logger logger{"language_settings_model"};
  int selected{};
  languages_model *languages{};
  QQmlEngine &engine;
  std::unique_ptr<QTranslator> translator;
};
} // namespace client

#endif
