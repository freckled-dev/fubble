import os
from conans import ConanFile

class LinuxArmv7Conan(ConanFile):
    name = "linux_armv7"
    version = "1.0"
    settings = "os", "arch", "compiler", "build_type"
    no_copy_source = True
    default_user = "acof"
    default_channel = "stable"
    _sysroot_folder_name = "debian_sid_arm-sysroot"

    def source(self):
        self.run('mkdir sysroot_scripts')
        script_file = ['install-sysroot.py', 'sysroots.json']
        for file in script_file:
            self.run('curl -L https://chromium.googlesource.com/chromium/src/+/lkgr/build/linux/sysroot_scripts/{}?format=TEXT | base64 -d > sysroot_scripts/{}'.format(file, file))
        self.run('python3 sysroot_scripts/install-sysroot.py --arch arm')

    def package(self):
        # Copy all the required files for your toolchain
        self.copy("*", dst=self._sysroot_folder_name, src=self._sysroot_folder_name, symlinks=True)

    def package_info(self):
        sysroot_folder = os.path.join(self.package_folder, self._sysroot_folder_name)
        target = 'arm-linux-gnueabihf'
        self.env_info.CC = "clang"
        self.env_info.CXX = "clang++"
        self.env_info.AR = "llvm-ar"
        self.env_info.STRIP = "llvm-strip"
        self.env_info.RANLIB = "llvm-ranlib"
        self.env_info.SYSROOT = sysroot_folder
        self.env_info.CFLAGS = '--sysroot="{}" -target {}'.format(sysroot_folder, target)
        self.env_info.CXXFLAGS = '--sysroot="{}" -target {}'.format(sysroot_folder, target)
        self.env_info.LDFLAGS = '--sysroot="{}" -target {}'.format(sysroot_folder, target)

