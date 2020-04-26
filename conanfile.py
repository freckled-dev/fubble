from conans import ConanFile, Meson, tools


class FubbleConan(ConanFile):
    name = "fubble"
    version = "1.0"
    license = "All Rights Reserved"
    author = "Fubble OG <contact@fubble.io>"
    url = "fubble.io"
    description = "Conferencing that works"
    topics = ("conference", "fubble", "video", "audio", "webrtc")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "pkg_config"
    exports_sources = "*"
    # no_copy_source = True

    def imports(self):
        self.copy("*.dll", dst="bin", keep_path=False)

    def build_requirements(self):
        #if self.settings.os != "Linux":
        #    # qt/5.14.2 is available but conan can't find the prebuild dependencies
        #    # there's no out of the box support for qml. and compiling it yourself fails.
        #    # TODO find out why self compile does not work and contribute
        #    self.build_requires("qt/5.14.0@bincrafters/stable")
        self.build_requires("nlohmann_json/3.7.0")
        self.build_requires("boost-di/1.1.0@inexorgame/stable")
        self.build_requires("boost/1.72.0")
        self.build_requires("gtest/1.10.0")
        self.build_requires("fmt/6.2.0")
        self.build_requires("google-webrtc/m79")

    def build(self):
        # https://docs.conan.io/en/latest/reference/build_helpers/meson.html
        #qt_path_bin = self.deps_cpp_info["qt"].bin_paths
        qt_path_bin = 'C:\\Qt\\5.14.2\\msvc2017_64\\bin'
        self.output.info(f"qt_path_bin:{qt_path_bin}")

        boost_path = self.deps_cpp_info["boost"].rootpath
        self.output.info(f"boost_path:{boost_path}")
        boost_include_path = self.deps_cpp_info["boost"].include_paths
        self.output.info(f"boost_include_path:{boost_include_path}")
        boost_library_path = self.deps_cpp_info["boost"].lib_paths
        self.output.info(f"boost_library_path:{boost_library_path}")
        meson = Meson(self)
        with tools.environment_append({
                "PATH": [qt_path_bin],
                "BOOST_ROOT": boost_path, 
                "BOOST_INCLUDEDIR": boost_include_path,
                "BOOST_LIBRARYDIR": boost_library_path}):
            meson.configure(build_folder="builddir",
                defs={'cpp_std': 'c++17'})
        # meson.build(args=["-j1"])
        # meson.build(args=["-k0"])
        meson.build()

    def package(self):
        self.copy("*.hpp", dst="include")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("*.exe", dst="bin", keep_path=False)

    def package_info(self):
        pass
