# building

ensure python2 is enabled. the scripts do NOT work for python3.

list possible arguments
```
gn args --list out/my_build
```
notable flags: `rtc_use_x11`, `rtc_use_h264`, `rtc_use_gtk`

get help for a variable by using `gn args help <arg>`, eg `gn args help target_cpu`

## linux

dependencies: run `install-build-deps.sh` or checkout https://chromium.googlesource.com/chromium/src/+/master/docs/linux_build_instructions.md#notes

because of the abi change https://docs.conan.io/en/latest/howtos/manage_gcc_abi.html#how-to-manage-the-gcc-5-abi
the define `_GLIBCXX_USE_CXX11_ABI` must be set.
`use_custom_libcxx_for_host` and `use_custom_libcxx` do that.
https://stackoverflow.com/questions/47348330/error-linking-webrtc-native-due-to-undefined-reference-to-methods-having-stdst/47384787

### x64
```bash
gn gen out/Default --args='is_debug=true use_custom_libcxx=false use_custom_libcxx_for_host=false cc_wrapper="ccache" use_rtti=true'
```

### 32 bit
`target_cpu="x86"`

### arm
https://gn.googlesource.com/gn/+/master/docs/quick_start.md
https://gn.googlesource.com/gn/+/master/docs/cross_compiles.md

```bash
./build/linux/sysroot_scripts/install-sysroot.py --arch=arm
gn gen out/arm --args='target_os="linux" target_cpu="arm" is_debug=true cc_wrapper="ccache" use_rtti=true use_custom_libcxx_for_host=false'
# arm64
./build/linux/sysroot_scripts/install-sysroot.py --arch=arm64
gn gen out/arm64 --args='target_os="linux" target_cpu="arm64" is_debug=true cc_wrapper="ccache" use_rtti=true use_custom_libcxx_for_host=false'
```

# Notes

if you `close()` on the `RTCConnection` the other end will timeout after 20 seconds.
the other side will not call any event.

## threading

native information: http://webrtc.github.io/webrtc-org/native-code/native-apis/

`peer_factory` immediately posts to signaling thread. if signaling thread is not running, it will block!

> Calls to the Stream APIs and the PeerConnection APIs will be proxied to the signaling thread, which means that an application can call those APIs from whatever thread.


# links

- bugs/issues https://bugs.chromium.org/p/webrtc/issues/list

