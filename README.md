# set up

```bash
pip3 install --user conan meson==0.51.2
conan remote add inexorgame https://api.bintray.com/conan/inexorgame/inexor-conan
# fedora
sudo dnf install -y \
  gstreamer1-plugins-bad-free-devel \
  libasan \
  libX11-devel
# opensuse
sudo zypper install libX11-devel
```

if using fish add the python3 bin path to PATH
```
> cat ~/.config/fish/config.fish
set -x PATH $PATH $HOME/.local/bin
```

fix gcc >= 5 ABI. https://docs.conan.io/en/latest/howtos/manage_gcc_abi.html
Change conan config file `~/.conan/profiles/default` and set `compiler.libcxx=libstdc++11`.


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

