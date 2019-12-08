# building

ensure python2 is enabled. the scripts do NOT work for python3.

list possible arguments
```
gn args --list out/my_build
```

## linux

dependencies: run `install-build-deps.sh` or checkout https://chromium.googlesource.com/chromium/src/+/master/docs/linux_build_instructions.md#notes

### x64
```bash
gn gen out/Default --args='is_debug=true use_custom_libcxx=false use_custom_libcxx_for_host=false cc_wrapper="ccache" use_rtti=true'
```

### arm
https://gn.googlesource.com/gn/+/master/docs/quick_start.md
https://gn.googlesource.com/gn/+/master/docs/cross_compiles.md

```bash
./build/linux/sysroot_scripts/install-sysroot.py --arch=arm
gn gen out/arm --args='target_os="linux" target_cpu="arm" is_debug=true cc_wrapper="ccache" use_rtti=true'
# arm64
./build/linux/sysroot_scripts/install-sysroot.py --arch=arm64
gn gen out/arm64 --args='target_os="linux" target_cpu="arm64" is_debug=true cc_wrapper="ccache" use_rtti=true'
```

# Notes

if you `close()` on the `RTCConnection` the other end will timeout after 20 seconds.
the other side will not call any event.

# links

- bugs/issues https://bugs.chromium.org/p/webrtc/issues/list

