#ifndef UUID_C0B2292C_B876_49E2_BC24_D7935D1C1F06
#define UUID_C0B2292C_B876_49E2_BC24_D7935D1C1F06

#include <QQmlExtensionPlugin>

class plugin : public QQmlExtensionPlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "io.fubble")
public:
  void registerTypes(const char *uri) override;
};

#endif
