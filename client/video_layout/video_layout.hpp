#ifndef UUID_75FCFD10_E735_44D4_9F6A_5AC7C9E72988
#define UUID_75FCFD10_E735_44D4_9F6A_5AC7C9E72988

#include <QQuickItem>

class video_layout : public QQuickItem {
  Q_OBJECT
public:
  video_layout(QQuickItem *parent = nullptr);
  void itemChange(QQuickItem::ItemChange change,
                  const QQuickItem::ItemChangeData &value) override;

protected:
  void on_child_added(const QQuickItem::ItemChangeData &value);
  void recalculate();
};

#endif
