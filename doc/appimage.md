# appimage

- appimage docs: https://docs.appimage.org
- conan: https://docs.conan.io/en/latest/integrations/deployment/appimage.html
- linuxdeploy: https://github.com/linuxdeploy/linuxdeploy If you use linuxdeployqt at the moment, consider switching to linuxdeploy today!
- about desktop files https://developer.gnome.org/integration-guide/stable/desktop-files.html.en
- desktop files categories https://specifications.freedesktop.org/menu-spec/menu-spec-1.0.html#category-registry

## setup docker ubuntu 18.04

run container as `--priviledged`, else `fuse` won't work.

```bash
apt-get install fuse \
  qml-module-qtquick-\* qml-module-qt-labs-\* \
  qml-module-qtcharts qml-module-qtmultimedia \
  qtdeclarative5-dev-tools 
curl -LO https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
curl -LO https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
chmod u+x linuxdeploy-x86_64.AppImage linuxdeploy-plugin-qt-x86_64.AppImage
export QML_SOURCES_PATHS=$PWD/client/app/
./linuxdeploy-x86_64.AppImage \
  --plugin qt \
  --executable ../fubble_build/install/bin/fubble \
  --desktop-file=client/app/fubble.desktop \
  --icon-file=client/app/images/fubble.svg
  --appdir appdir \
  --output fubble.AppImage
```

the `*desktop` file controls how the resulting appimage will be named.


