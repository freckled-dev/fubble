from conans import ConanFile, Meson, tools
from conans.errors import ConanInvalidConfiguration
import os

class FubbleConan(ConanFile):
    name = "fubble"
    version = "1.0"
    license = "All Rights Reserved"
    author = "Fubble OG <contact@fubble.io>"
    url = "fubble.io"
    description = "Conferencing that works"
    topics = ("conference", "fubble", "video", "audio", "webrtc")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [False, True], "treat_warnings_as_errors": [True, False],
            "sanatize": [True, False]}
    # https://docs.conan.io/en/latest/reference/conanfile/attributes.html#default-options
    default_options = {"shared": False, "nlohmann_json:multiple_headers": True,
            "treat_warnings_as_errors": False, "sanatize": False}
    generators = "pkg_config"
    exports_sources = "*"
    # no_copy_source = True

    def _get_qt_bin_paths(self):
        if self.settings.os != "Windows":
            return []
        return ['C:\\Qt\\5.15.0\\msvc2019_64\\bin']
        # return self.deps_cpp_info["qt"].bin_paths

    def _get_build_type(self):
        return self.settings.get_safe("build_type", default="Release")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.sanatize

    def configure(self):
        if self._get_build_type() == "Release" and 'sanatize' in self.options:
            if self.options.sanatize:
                raise ConanInvalidConfiguration("can't enable sanatize without debug information")

    def imports(self):
        self.copy("*.dll", dst="bin", keep_path=False)

    def build_requirements(self):
        # if self.settings.os == "Windows":
        #     # will not compile with less than visual studio 2019
        #     self.build_requires("qt/5.15.0@bincrafters/stable")
        if not tools.which('meson'):
            self.build_requires("meson/0.55.0")
        self.build_requires("nlohmann_json/3.7.0")
        self.build_requires("boost/1.73.0")
        self.build_requires("gtest/1.10.0")
        self.build_requires("fmt/7.0.3")
        self.build_requires("google-webrtc/84")
        self.build_requires("RectangleBinPack/1.0.2")
        self.build_requires("fruit/3.6.0")

    def build(self):
        # https://docs.conan.io/en/latest/reference/build_helpers/meson.html
        addtional_paths = []
        if self.settings.os == "Windows":
            qt_path_bin = self._get_qt_bin_paths()
            self.output.info("qt_path_bin:%s" % (qt_path_bin))
            addtional_paths += qt_path_bin

        boost_path = self.deps_cpp_info["boost"].rootpath
        self.output.info("boost_path:%s" % (boost_path))
        boost_include_path = self.deps_cpp_info["boost"].include_paths
        self.output.info("boost_include_path:%s" % (boost_include_path))
        boost_library_path = self.deps_cpp_info["boost"].lib_paths
        self.output.info("boost_library_path:%s" % (boost_library_path))

        with_servers = False
        with_tests = True
        if self.settings.os == "Windows":
            with_tests = False
        if self.settings.os == "Linux":
            with_servers = True

        # https://mesonbuild.com/Builtin-options.html#base-options
        meson_options = {'cpp_std': 'c++17', 'b_ndebug': 'if-release',
                        'with_servers': with_servers, 'with_tests': with_tests}
        if self.settings.os == "Linux":
            meson_options['warning_level'] = '3'
        if self.options.treat_warnings_as_errors:
            meson_options['werror'] = 'true'
        build_type = self._get_build_type()
        if build_type == 'Debug' and self.settings.os == 'Linux':
            meson_options['b_lto'] = 'false'
        if 'sanatize' in self.options and self.options.sanatize:
            meson_options['b_sanitize'] = 'address,undefined'
        meson_options['b_pch'] = 'false'
        # meson_options['b_vscrt'] = 'mtd'

        ninja_jobs = os.getenv('FUBBLE_BUILD_NINJA_JOBS')
        meson = Meson(self)
        with tools.environment_append({
                "PATH": addtional_paths,
                "BOOST_ROOT": boost_path,
                "BOOST_INCLUDEDIR": boost_include_path,
                "BOOST_LIBRARYDIR": boost_library_path}):
            meson.configure( build_folder="meson", defs=meson_options)
            build_args = []
            if ninja_jobs:
                build_args += ['-j %s' % (ninja_jobs)]
            meson.build(args=build_args)
            # meson.build(args=["-k0"])
            # meson.build()

    def package(self):
        meson = Meson(self)
        meson.install(build_dir="meson")
        if self.settings.os == "Windows":
            bin_dir = os.path.join(self.package_folder, 'bin')
            vars_dict = tools.vcvars_dict(self.settings)

            windows_sdk_dir = vars_dict['WindowsSdkDir']
            windows_sdk_version = vars_dict['WindowsSDKVersion']
            ucrt_redist_dir = os.path.join(windows_sdk_dir, 'Redist', windows_sdk_version, 'ucrt', 'DLLs', 'x64')
            if not os.path.exists(ucrt_redist_dir):
                raise ConanInvalidConfiguration("ucrt redist dir does not exist: %s" % (ucrt_redist_dir))
            self.copy('*.dll', dst=bin_dir, src=ucrt_redist_dir)

            vctoolsredist_dir = vars_dict['VCToolsRedistDir']
            vcredist_dir = os.path.join(vctoolsredist_dir, 'x64', 'Microsoft.VC142.CRT')
            if not os.path.exists(vcredist_dir):
                raise ConanInvalidConfiguration("ucrt redist dir does not exist: %s" % (ucrt_redist_dir))
            self.copy('*.dll', dst=bin_dir, src=vcredist_dir)

            qt_path_bin = self._get_qt_bin_paths()
            with tools.environment_append({"PATH": qt_path_bin}):
                with tools.chdir(bin_dir):
                    qml_dir = os.path.join(self.source_folder, 'client', 'app')
                    # dont do -no-widgets # widgets is needed for svg
                    self.run('windeployqt.exe fubble.exe --no-compiler-runtime --qmldir "%s"'
                        % (qml_dir))

    def package_info(self):
        pass
