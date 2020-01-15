# set up

```bash
pip3 install --user conan meson==0.51.2
# boost-di
conan remote add inexorgame https://api.bintray.com/conan/inexorgame/inexor-conan
# qt
conan remote add bincrafters "https://api.bintray.com/conan/bincrafters/public-conan"

# fedora
sudo dnf install -y \
  gstreamer1-plugins-bad-free-devel \
  libasan \
  libX11-devel \
  qt5-devel \
  qt5-qtquickcontrols2-devel

# opensuse
sudo zypper install libX11-devel cmake gcc-c++ git meson
```

if using fish add the python3 bin path to PATH
```
> cat ~/.config/fish/config.fish
set -x PATH $PATH $HOME/.local/bin
```

fix gcc >= 5 ABI. https://docs.conan.io/en/latest/howtos/manage_gcc_abi.html
Change conan config file `~/.conan/profiles/default` and set `compiler.libcxx=libstdc++11`.

# notes on qt5

## logging and fedora

by default it's disabled to log with `console.log` and `console.debug`. other logs still work, eg: `console.info`.
https://stackoverflow.com/questions/36338000/qml-console-log-and-console-debug-dont-write-to-console

## qml

When using the `Q_PROPERTY` I have to give the full typename with all namespaces noted.
Don't do `typedef`s or `using`s.
https://stackoverflow.com/questions/19889163/using-qt-properties-with-custom-types-defined-in-namespaces
https://wiki.qt.io/How_to_use_a_C_class_declared_in_a_namespace_with_Q_PROPERTY_and_QML

# conan
enable `_GLIBCXX_USE_CXX11_ABI` by setting `compiler.libcxx=libstdc++11` in `~/.conan/profiles/default`

# python2 is mandatory

```
sudo dnf install -y \
  python2
pip2 install --user virtualenv
python2 -m virtualenv venv
source venv/bin/activate.fish
```

# dependency google webrtc

seems to not work with fedora. Tested on ubuntu 18.04 and 19.04

```fish
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
set PATH $PATH /home/mlanner/Development/projects/fubble/google_webrtc/depot_tools/
# dont skip `--nohooks`
# if `fetch` fails. delete all files (including the hidden ones) and run again.
fetch --nohooks webrtc
git checkout -b my_branch refs/remotes/branch-heads/m79
gclient sync
```

list all build args
```
gn args --list out/Default
# the following line will bring up an editor with the set values
# more information: https://chromium.googlesource.com/chromium/src/tools/gn/+/48062805e19b4697c5fbd926dc649c78b6aaa138/docs/quick_start.md
gn args out/Default
```

# v4l2loopback

source: https://github.com/umlaeute/v4l2loopback

```bash
# preconditions
sudo dnf install kernel-devel
# build
git clone https://github.com/umlaeute/v4l2loopback.git
cd v4l2loopback
make
sudo make install
# load
# gstreamer can't allocate unless `max_buffers` is set.
# https://github.com/umlaeute/v4l2loopback/issues/166#issuecomment-465960256
sudo modprobe v4l2loopback max_buffers=2
```

send videos to fake device

- ffmpeg: https://github.com/umlaeute/v4l2loopback/wiki/Ffmpeg
- gstreamer: webrtc could not read it https://github.com/umlaeute/v4l2loopback/wiki/Gstreamer

```bash
# produce a video
# sintel https://durian.blender.org/download/
curl -LO http://peach.themazzone.com/durian/movies/sintel-1280-surround.mp4
ffmpeg -re -i ./sintel-1280-surround.mp4 -f v4l2 /dev/video0

# consume it by gstreamer
gst-launch-1.0 v4l2src device=/dev/video0 ! videoconvert ! autovideosink
```

