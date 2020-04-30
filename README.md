# set up

```bash
# ubuntu 18.04
sudo apt-get update
sudo apt-get install fish
./scripts/install_system_dependencies
FUBBLE_TREAT_WARNING_AS_ERROR=0 ./scripts/make_build.py

# ubuntu 14.04
sudo apt-get update
sudo apt-get install -y python3.5 curl
curl -L https://bootstrap.pypa.io/get-pip.py -o get-pip.py
python3.5 get-pip.py
python3.5 -m pip install --user conan
export PATH="$HOME/.local/bin:$PATH
sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install -y g++-9
conan profile update settings.compiler.version=9 default
conan profile update settings.compiler.libcxx=libstdc++11 default

curl -LO https://raw.githubusercontent.com/freckled-dev/conan-google-webrtc/master/install-build-deps.sh
./install-build-deps.sh --no-nacl --no-chromeos-fonts
sudo dpkg --add-architecture i386

# fedora
sudo dnf install -y \
  gstreamer1-plugins-bad-free-devel \
  libasan \
  libX11-devel \
  qt5-devel \
  qt5-qtquickcontrols2-devel

# opensuse
sudo zypper install libX11-devel cmake gcc-c++ git libqt5-qttools \
  libqt5-qtsvg-devel
```

## build

```
make
```

## set up servers for local testing

```bash
make install
./deploy/development/deploy_build_install_foreground
```

## run tests

```bash
# unit and integration tests
make test
# gui
../fubble_build/meson/client/app/client_gui
```

## markus notes - ignore

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
Same applies to signals.
Don't do `typedef`s or `using`s.
- https://stackoverflow.com/questions/19889163/using-qt-properties-with-custom-types-defined-in-namespaces
- https://wiki.qt.io/How_to_use_a_C_class_declared_in_a_namespace_with_Q_PROPERTY_and_QML
- https://stackoverflow.com/questions/24231470/qmetapropertyread-unable-to-handle-unregistered-datatype-treeiteminspectori

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

## linux

seems to not work with fedora. Tested on ubuntu 18.04 and 19.04

```fish
apt install -y git wget curl
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
// TODO more movie links. maybe with torrent: https://www.maketecheasier.com/how-to-download-torrents-from-the-command-line-in-ubuntu/
ffmpeg -re -i ./sintel-1280-surround.mp4 -f v4l2 /dev/video0

# consume it by gstreamer
gst-launch-1.0 v4l2src device=/dev/video0 ! videoconvert ! autovideosink
```

## windows

instructions for webrtc
https://chromium.googlesource.com/chromium/src/+/master/docs/windows_build_instructions.md

use the visual studio installer and install the “Desktop development with C++”
component and the “MFC/ATL support” sub-components.

The SDK Debugging Tools must also be installed. If the Windows 10 SDK was 
installed via the Visual Studio installer, then they can be installed by going
to: Control Panel → Programs → Programs and Features → 
Select the “Windows Software Development Kit” → Change → Change → 
Check “Debugging Tools For Windows” → Change.


because there's no pip, install conan by installer \
https://conan.io/downloads.html
https://dl.bintray.com/conan/installers/conan-win-64_1_24_1.exe

install git and python3 (ensure python3 and pip3 become part of PATH)
- https://git-scm.com/download/win
- https://www.python.org/ftp/python/3.8.2/python-3.8.2.exe

install pkg-config
- https://sourceforge.net/projects/pkgconfiglite/

install cmake for, needed by qt dependencies

add conan remotes
```bat
conan remote add bincrafters "https://api.bintray.com/conan/bincrafters/public-conan"
conan remote add inexorgame "https://api.bintray.com/conan/inexorgame/inexor-conan"
conan remote add google_webrtc "https://api.bintray.com/conan/freckled/google-webrtc"
```

build using conan
```bat
conan create . --build missing
```

