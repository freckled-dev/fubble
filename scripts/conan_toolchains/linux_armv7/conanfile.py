import os
from conans import ConanFile, tools

class LinuxArmv7Conan(ConanFile):
    name = "linux_armv7"
    version = "1.0"
    settings = "os", "arch" 
    no_copy_source = True
    default_user = "acof"
    default_channel = "stable"
    _sysroot_folder_name = "debian_sid_arm-sysroot"
    # exports_sources = "*cmake"

    def source(self):
        self.run('mkdir sysroot_scripts')
        script_file = ['install-sysroot.py', 'sysroots.json']
        for file in script_file:
            self.run('curl -L https://chromium.googlesource.com/chromium/src/+/lkgr/build/linux/sysroot_scripts/{}?format=TEXT | base64 -d > sysroot_scripts/{}'.format(file, file))
        self.run('python3 sysroot_scripts/install-sysroot.py --arch arm')

    def build(self):
        pass

    def package(self):
        # Copy all the required files for your toolchain
        self.copy("*", dst=self._sysroot_folder_name, src=self._sysroot_folder_name, symlinks=True)
        self.copy("*.cmake", dst='', src='')

    def package_info(self):
        sysroot_folder = os.path.join(self.package_folder, self._sysroot_folder_name)
        target = 'arm-linux-gnueabihf'
        self.env_info.CC = "clang"
        self.env_info.CXX = "clang++"
        self.env_info.AR = "llvm-ar"
        self.env_info.STRIP = "llvm-strip"
        self.env_info.RANLIB = "llvm-ranlib"
        self.env_info.SYSROOT = sysroot_folder
        self.env_info.CONAN_CMAKE_FIND_ROOT_PATH = sysroot_folder
        self.env_info.CONAN_CMAKE_SYSROOT = sysroot_folder
        self.env_info.PKG_CONFIG_SYSROOT_DIR = sysroot_folder
        self.env_info.PKG_CONFIG_LIBDIR = [
            '{}/usr/lib/pkgconfig'.format(sysroot_folder),
            '{}/usr/share/pkgconfig'.format(sysroot_folder),
            '{}/usr/lib/arm-linux-gnueabihf/pkgconfig'.format(sysroot_folder),
        ]
        self.env_info.CFLAGS = '--sysroot="{}" -target {}'.format(sysroot_folder, target)
        self.env_info.CXXFLAGS = '--sysroot="{}" -target {}'.format(sysroot_folder, target)
        self.env_info.LDFLAGS = '--sysroot="{}" -target {} -rpath {} -rpath {} -latomic'.format(
            sysroot_folder, target, '/lib/arm-linux-gnueabihf/', '/usr/lib/arm-linux-gnueabihf/')

        toolchain_file = os.path.join(self.package_folder, "toolchain.cmake")
        # https://cmake.org/cmake/help/book/mastering-cmake/chapter/Cross%20Compiling%20With%20CMake.html
        toolchain_content = """
            set(CMAKE_FIND_ROOT_PATH {})
            set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
            set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
            set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
            # https://cmake.org/cmake/help/latest/module/GoogleTest.html
            set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
            """.format(sysroot_folder)
        tools.save(toolchain_file, toolchain_content)
        self.conf_info["tools.cmake.cmaketoolchain:user_toolchain"] = toolchain_file

