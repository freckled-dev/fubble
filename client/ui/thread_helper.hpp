#ifndef UUID_F2F98C48_E854_471A_8113_D1ABABB7633D
#define UUID_F2F98C48_E854_471A_8113_D1ABABB7633D

#include <QGuiApplication>
#include <QtCore/QAbstractEventDispatcher>

// https://stackoverflow.com/questions/21646467/how-to-execute-a-functor-or-a-lambda-in-a-given-thread-in-qt-gcd-style
template <typename F> static void post_to_object(F &&fun, QObject *obj = qApp) {
  QMetaObject::invokeMethod(obj, std::forward<F>(fun));
}

template <typename F>
static void post_to_thread(F &&fun, QThread *thread = qApp->thread()) {
  auto *obj = QAbstractEventDispatcher::instance(thread);
  Q_ASSERT(obj);
  QMetaObject::invokeMethod(obj, std::forward<F>(fun));
}

#endif
