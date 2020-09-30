#ifndef UUID_75FCFD10_E735_44D4_9F6A_5AC7C9E72988
#define UUID_75FCFD10_E735_44D4_9F6A_5AC7C9E72988

#include <QQuickItem>

// TODO `namespace client`
class video_layout : public QQuickItem {
  Q_OBJECT
public:
  video_layout(QQuickItem *parent = nullptr);
  void itemChange(QQuickItem::ItemChange change,
                  const QQuickItem::ItemChangeData &value) override;

public slots:
  void on_aspect_changed();
  void recalculate();

protected:
  void on_child_added(const QQuickItem::ItemChangeData &value);
};

#endif
