# Fubble - fast realtime multimedia communication

There are two parts:
- "fubble" is a conferencing solution. It extends the [matrix standard](https://matrix.org/) with a many-to-many webrtc solution.
- "libfubble" is a library that can be used in other products as webrtc-as-a-service. It solves the signaling and the communication challenges.

Unlike other solutions it's implemented in a realtime language,
so it can be used on low-powered devices, like phones and Raspberry Pis.

Get the prebuild binaries at [fubble.io](https://fubble.io).

![Screenshot](https://fubble.io/assets/images/screenshots/main.png "Welcome Screen")

# Build Status

[![GitlabCI](https://gitlab.com/acof/fubble/badges/master/pipeline.svg)](https://gitlab.com/acof/fubble/pipelines)

# Get the source

- Gitlab [gitlab.com/acof/fubble](https://gitlab.com/acof/fubble)
- Github [github.com/freckled-dev/fubble](https://github.com/freckled-dev/fubble)

# Build from source

Fubble uses mainly these heavy frameworks:
- [google webrtc](https://webrtc.googlesource.com/src/+/refs/heads/master/docs/native-code/index.md)
- [qt5](https://qt.io)
- [boost](https://boost.org)
- [Googletest](https://github.com/google/googletest)

The build system uses:
- [meson](https://mesonbuild.com/) for building fubble. (build system)
- [aqtinstall](https://github.com/miurahr/aqtinstall) for installing qt5
- [conan](https://conan.io) for intalling all other dependencies, including webrtc.

## Ubuntu 20.04 Desktop

### install system dependencies

```bash
sudo apt install python3 python-is-python3 python3-pip ninja-build cmake ccache
```

### install the webrtc dependencies

```bash
curl -LO 'https://raw.githubusercontent.com/chromium/chromium/master/build/install-build-deps.sh'
chmod u+x install-build-deps.sh 
./install-build-deps.sh --no-arm --no-chromeos-fonts --no-nacl
```



### install project build systems

```bash
pip3 install --user conan meson aqtinstall
# ensure `$HOME/.local/bin/` is in your `PATH`
which conan
```

### build and install

```bash
./scripts/make_build.py --profile ./scripts/conan_profiles/linux64_gcc9_release
```

This command will result in a ready to use fubble client. The binaries can be found in `../fubble_build/install/bin`.

Run it using `LD_LIBRARY_PATH=../fubble_build/qt/5.15.1/gcc_64/lib/ ../fubble_build/install/bin/fubble`.

## MacOS

You need to install the newest XCode on Mojave. Next install the build helpers
```
brew install cmake ninja ccache python
pip3 install --user conan meson aqtinstall
```

### build

```bash
./scripts/make_build.py --profile ./scripts/conan_profiles/mac_release
```

The MacOS version is a work-in-progress. There is currently no video support.
To run it you have to use `--use-video 0`, like this:
```bash
../fubble_build/install/bin/fubble --use-video 0
```

## Windows

Set up your PC according to the [Chrome Building Instructions](https://chromium.googlesource.com/chromium/src/+/master/docs/windows_build_instructions.md#visual-studio).
You need at least Visual Studio 2019.

Install `python3`, `cmake` and `pkgconfiglite` using [chocolatey](https://chocolatey.org/) in a powershell with Administration Rights
```powershell
choco install python pkgconfiglite cmake --installargs 'ADD_CMAKE_TO_PATH=System'
# load PATH
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
pip install conan aqtinstall
```

At last install Qt to `C:\Qt` using the script `install_qt.py`
```powershell
python scripts/install_qt.py
```

### Build

```powershell
python scripts/make_build.py --profile scripts/conan_profiles/windows_release
```

You can find the resulting client at `..\fubble_build\install\bin`

# Set up servers for local testing and deployment

The only supported platform for the servers is [Docker](https://docker.io). Install it using `apt install docker.io`.

To run your own instances of the servers, build fubble then run

```bash
cd docker
docker-compose up
```

Use `docker-compose up --build` to update the containers.
Use `docker-compose up --detach` to detach and run the services in the background.

Connect to them using fubble:
```bash
../fubble_build/install/bin/fubble --host localhost --service http --use-ssl 0
```

## ports

To change port of the backend edit `./docker/docker-compose.yml` file and adapt
the first value of the `ports` entry `"80:80"` to your liking.
For example change to `"8000:80"`.

