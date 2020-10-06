#ifndef UUID_75FCFD10_E735_44D4_9F6A_5AC7C9E72988
#define UUID_75FCFD10_E735_44D4_9F6A_5AC7C9E72988

#include <QQuickItem>

class video_layout : public QQuickItem {
  Q_OBJECT
  Q_PROPERTY(
      QString layout READ get_layout WRITE set_layout NOTIFY layout_changed)
public:
  video_layout(QQuickItem *parent = nullptr);
  void itemChange(QQuickItem::ItemChange change,
                  const QQuickItem::ItemChangeData &value) override;

signals:
  void layout_changed(QString);

public slots:
  void on_aspect_changed();
  void recalculate();

protected:
  Q_INVOKABLE void enlarge(QQuickItem *item);
  Q_INVOKABLE void shrink(QQuickItem *item);
  QString get_layout() const;
  void set_layout(QString set);
  void on_child_added(const QQuickItem::ItemChangeData &value);
  void clear_focused();

  enum class layout { grid, enlarged, full };
  layout layout_{layout::grid};
  QQuickItem *focused{};
};

#endif
