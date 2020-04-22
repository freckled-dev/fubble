#ifndef UUID_F2F98C48_E854_471A_8113_D1ABABB7633D
#define UUID_F2F98C48_E854_471A_8113_D1ABABB7633D

#include <QtCore/QAbstractEventDispatcher>
#include <boost/assert.hpp>

// https://stackoverflow.com/questions/21646467/how-to-execute-a-functor-or-a-lambda-in-a-given-thread-in-qt-gcd-style
// https://stackoverflow.com/questions/24899558/how-to-check-qt-version-to-include-different-header/29887388#29887388
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
template <typename F> void post_to_object(F &&fun, QObject *obj) {
  [[maybe_unused]] bool result =
      QMetaObject::invokeMethod(obj, std::forward<F>(fun));
  BOOST_ASSERT(result);
}

template <typename F> void post_to_thread(F &&fun, QThread *thread) {
  auto *obj = QAbstractEventDispatcher::instance(thread);
  Q_ASSERT(obj);
  QMetaObject::invokeMethod(obj, std::forward<F>(fun));
}
#else
#include <QtCore/QCoreApplication>
#include <QtCore/QThread>
namespace detail {
template <typename F> struct post_event : public QEvent {
  using Fun = typename std::decay<F>::type;
  Fun fun;
  post_event(Fun &&fun) : QEvent(QEvent::None), fun(std::move(fun)) {}
  post_event(const Fun &fun) : QEvent(QEvent::None), fun(fun) {}
  ~post_event() { fun(); }
};
} // namespace detail

template <typename F> void post_to_object(F &&fun, QObject *obj) {
  BOOST_ASSERT(
      qobject_cast<QThread *>(obj) == nullptr &&
      "posting a call to a thread object - consider using postToThread");
  QCoreApplication::postEvent(obj,
                              new detail::post_event<F>(std::forward<F>(fun)));
}

template <typename F> static void postToThread(F &&fun, QThread *thread) {
  QObject *obj = QAbstractEventDispatcher::instance(thread);
  Q_ASSERT(obj);
  QCoreApplication::postEvent(obj,
                              new detail::post_event<F>(std::forward<F>(fun)));
}
#endif

#endif
