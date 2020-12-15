from conans import ConanFile, Meson, tools
from six import StringIO
from conans.errors import ConanInvalidConfiguration
from conans.tools import os_info
import os

class FubbleConan(ConanFile):
    name = "fubble"
    license = "All Rights Reserved"
    author = "Fubble OG <contact@fubble.io>"
    url = "fubble.io"
    description = "Conferencing that works"
    topics = ("conference", "fubble", "video", "audio", "webrtc")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [False, True],
            "treat_warnings_as_errors": [True, False],
            "sanatize": [True, False], "qt_install": "ANY",
            "enable_ui": [True, False], "meson_cross_file": "ANY"}
    # https://docs.conan.io/en/latest/reference/conanfile/attributes.html#default-options
    default_options = {"shared": False, "qt_install": None, "enable_ui": True,
            "nlohmann_json:multiple_headers": True,
            "restinio:asio": "boost",
            # qt options
            # "qt:openssl": False, "qt:with_mysql": False, "qt:with_pq": False, "qt:with_odbc": False, "qt:widgets": False,
            # "qt:with_libalsa": False,
            # qt submodules https://github.com/bincrafters/conan-qt/blob/testing/5.15.1/qtmodules.conf
            # "qt:qtsvg": True, "qt:qtmultimedia": True, "qt:qtquickcontrols2": True, "qt:qtcharts": True,
            "treat_warnings_as_errors": False, "sanatize": False}
    generators = "pkg_config"
    exports_sources = "*", "!client/app/mock_qml_models*"
    no_copy_source = True

    # https://docs.conan.io/en/latest/versioning/introduction.html
    version = '1.0.0'
    #def set_version(self):
    #    buffer = StringIO()
    #    self.run("git describe", output=buffer)
    #    self.version = buffer.getvalue()

    def _get_qt_bin_paths(self):
        if self.settings.os != "Windows":
            return []
        return ['C:\\Qt\\5.15.1\\msvc2019_64\\bin']
        # return self.deps_cpp_info["qt"].bin_paths

    def _is_ios(self):
            return self.settings.os == "iOS"

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

    #def source(self):
    #    self.run("git clone git@gitlab.com:acof/fubble.git .")

    def build_requirements(self):
        # if self.settings.os == "Windows":
        #     # will not compile with less than visual studio 2019
        #     self.build_requires("qt/5.15.0@bincrafters/stable")
        if not tools.which('meson'):
            self.build_requires("meson/0.56.0")
        if os_info.is_macos: # maybe even for windows, instead of the pkgconfig-lite "hack"
            self.build_requires("pkgconf/1.7.3")

    def requirements(self):
        self.requires("nlohmann_json/3.7.0")
        self.requires("boost/1.73.0")
        self.requires("gtest/1.10.0")
        self.requires("fmt/7.0.3")
        self.requires("google-webrtc/84")
        if not self._is_ios():
            self.requires("RectangleBinPack/1.0.2")
        if self.settings.os == "Linux":
            self.requires("restinio/0.6.11")
            # self.requires("qt/5.15.1@bincrafters/stable")

    def build(self):
        # https://docs.conan.io/en/latest/reference/build_helpers/meson.html
        addtional_paths = []
        if self.settings.os == "Windows":
            qt_path_bin = self._get_qt_bin_paths()
            self.output.info("qt_path_bin:%s" % (qt_path_bin))
            addtional_paths += qt_path_bin
        elif self.options.qt_install:
            qt_install = str(self.options.qt_install)
            self.output.info("qt_install:%s" % (qt_install))
            addtional_paths += [os.path.join(qt_install, 'bin')]

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
        with_ui = self.options.enable_ui == True
        if self._is_ios():
            with_servers = False
            with_tests = False
            with_ui = False

        # https://mesonbuild.com/Builtin-options.html#base-options
        meson_options = {'cpp_std': 'c++17', 'b_ndebug': 'if-release',
                        'with_servers': with_servers, 'with_tests': with_tests,
                        'with_ui': with_ui}
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

        meson = Meson(self)
        with tools.environment_append({
                "PATH": addtional_paths,
                "BOOST_ROOT": boost_path,
                "BOOST_INCLUDEDIR": boost_include_path,
                "BOOST_LIBRARYDIR": boost_library_path}):
            pkg_config_paths = [self.install_folder]
            if self.options.qt_install:
                pkg_config_paths += [os.path.join(str(self.options.qt_install), 'lib/pkgconfig')]
            meson_args = ['--fatal-meson-warnings']
            if self.options.meson_cross_file:
                cross_file = str(self.options.meson_cross_file)
                if not os.path.isabs(cross_file):
                    cross_file = os.path.abspath(os.path.join(self.source_folder, cross_file))
                self.output.info("cross_file %s" % cross_file)
                meson_args += ['--cross-file', cross_file]

            meson.configure( build_folder="meson", defs=meson_options,
                    args=meson_args,
                    pkg_config_paths=pkg_config_paths
                    )
            build_args = []
            ninja_jobs = os.getenv('FUBBLE_BUILD_NINJA_JOBS')
            if ninja_jobs:
                build_args += ['-j %s' % (ninja_jobs)]
            meson.build(args=build_args)
            # meson.build(args=["-k0"])
            # meson.build()

    def package(self):
        meson = Meson(self)
        meson.install(build_dir="meson")
        if self.settings.os == "Linux":
            pass
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
                    qml_dir = os.path.join(self.source_folder, 'fubble', 'app')
                    # dont do -no-widgets # widgets is needed for svg
                    self.run('windeployqt.exe fubble.exe --no-compiler-runtime --qmldir "%s"'
                        % (qml_dir))

    def package_info(self):
        self.cpp_info.libs = ['fubble']
        self.cpp_info.includedirs = ['include']
        self.cpp_info.cxxflags = [
            '-DBOOST_THREAD_VERSION=5',
            '-DBOOST_ASIO_SEPARATE_COMPILATION',
            '-DBOOST_BEAST_SEPARATE_COMPILATION',
            ]
        if self.settings.os == "Windows":
            self.cpp_info.system_libs = [
                'ole32.lib', 'dbgeng.lib',
                ]
