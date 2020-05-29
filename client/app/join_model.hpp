#ifndef UUID_5D0F470B_C872_46EB_8804_1972618A74E6
#define UUID_5D0F470B_C872_46EB_8804_1972618A74E6

#include "client/logger.hpp"
#include <QObject>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace client {
namespace ui {
class frame_provider_google_video_source;
}
class joiner;
class room_model;
class room;
class error_model;
class model_creator;
class own_media;
// TODO do we have to pause the frameprovider?!
class join_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(
      bool videoAvailable MEMBER video_available NOTIFY video_available_changed)
  Q_PROPERTY(client::ui::frame_provider_google_video_source *video READ
                 get_video NOTIFY video_changed)
public:
  join_model(model_creator &model_factory, error_model &error_model_,
             joiner &joiner_, own_media &own_media_);
  ~join_model();

  Q_INVOKABLE void join(const QString &room, const QString &name);
  ui::frame_provider_google_video_source *get_video() const;

signals:
  void video_available_changed(bool);
  void video_changed(ui::frame_provider_google_video_source *);
  void joined(client::room_model *room);
  void join_failed();

protected:
  void on_joined(boost::future<std::shared_ptr<room>> room_);

  client::logger logger{"join_model"};
  model_creator &model_factory;
  error_model &error_model_;
  joiner &joiner_;
  own_media &own_media_;
  boost::inline_executor qt_executor;
  bool video_available;
  ui::frame_provider_google_video_source *video{};
};
} // namespace client

// Q_DECLARE_METATYPE(client::join_model *)

#endif
