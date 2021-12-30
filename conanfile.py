from conans import ConanFile, Meson, tools
from six import StringIO
from conans.errors import ConanInvalidConfiguration
from conans.tools import os_info
# from conan.tools.meson import MesonToolchain
import os
import shutil

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
            "meson_cross_file": None,
            "restinio:asio": "boost",
            # qt options
            # "qt:openssl": False, "qt:with_mysql": False, "qt:with_pq": False, "qt:with_odbc": False, "qt:widgets": False,
            # "qt:with_libalsa": False,
            # qt submodules https://github.com/bincrafters/conan-qt/blob/testing/5.15.1/qtmodules.conf
            # "qt:qtsvg": True, "qt:qtmultimedia": True, "qt:qtquickcontrols2": True, "qt:qtcharts": True,
            "treat_warnings_as_errors": False, "sanatize": False}
    generators = "pkg_config"
    #exports_sources = "*", "!fubble/app/mock_qml_models*", "!.*"
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
        self.requires("boost/1.77.0")
        self.requires("sigslot/1.2.1")
        self.requires("gtest/1.11.0")
        self.requires("fmt/8.0.1")
        self.requires("google-webrtc/94@acof/stable")
        if self.settings.os == "Windows":
            self.requires("openssl/1.1.1l")
        if not self._is_ios() and self.options.enable_ui:
            self.requires("rectanglebinpack/cci.20210901")
        if self.settings.os == "Linux":
            self.requires("restinio/0.6.11")
            # self.requires("qt/5.15.1@bincrafters/stable")

    # does not work well... it's a very new feature (2020-12), revisit and do issues!
    #def generate(self):
    #    tc = MesonToolchain(self)
    #    tc.generate()

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
        if self._is_ios() and self.settings.arch != "x86_64": # no bitcode for simulator
            meson_options['b_bitcode'] = 'true'
        meson_options['b_pch'] = 'false'
        # meson_options['b_vscrt'] = 'mtd'

        with tools.environment_append({
                "PATH": addtional_paths,
                "BOOST_ROOT": boost_path,
                "BOOST_INCLUDEDIR": boost_include_path,
                "BOOST_LIBRARYDIR": boost_library_path
                }):
            pkg_config_paths = [self.install_folder]
            if self.options.qt_install:
                pkg_config_paths += [os.path.join(str(self.options.qt_install), 'lib/pkgconfig')]
            meson_args = ['--fatal-meson-warnings']
            if self.options.meson_cross_file:
                cross_file = str(self.options.meson_cross_file)
                if not os.path.isabs(cross_file):
                    cross_file = os.path.abspath(os.path.join(self.source_folder, cross_file))
                cross_file_copy = os.path.join(self.install_folder, 'meson_cross.ini')
                shutil.copyfile(cross_file, cross_file_copy)
                tools.replace_in_file(cross_file_copy,
                    '__SYSROOT__', os.environ.get("SYSROOT", ""),
                    strict=False)
                tools.replace_in_file(cross_file_copy,
                    '__CC__', os.environ.get("CC", ""),
                    strict=False)
                tools.replace_in_file(cross_file_copy,
                    '__CXX__', os.environ.get("CXX", ""),
                    strict=False)
                tools.replace_in_file(cross_file_copy,
                    '__AR__', os.environ.get("AR", ""),
                    strict=False)
                tools.replace_in_file(cross_file_copy,
                    '__STRIP__', os.environ.get("STRIP", ""),
                    strict=False)
                self.output.info("cross_file %s" % cross_file_copy)
                meson_args += ['--cross-file', cross_file_copy]

                # this is a very dirty hack. for pkgconf. https://github.com/mesonbuild/meson/issues/8448
                self.run('ln -fs /home $SYSROOT || true')
                self.run('ln -fs /root $SYSROOT || true')

            if self.settings.compiler == "Visual Studio":
                self.output.info("using vcvars")
                with tools.vcvars(self):
                    self._build_meson(pkg_config_paths, meson_args)
            else:
                self._build_meson(pkg_config_paths, meson_args)

    def _get_meson_build_type(self):
        map = {
            "RelWithDebInfo": "debugoptimized",
            "Debug": "debug",
            "Release": "release"
        }
        result = map[self._get_build_type()]
        if result == None:
            raise ConanInvalidConfiguration("unsupported buildtype: %s" % (self._get_build_type()))
        return result


    def _build_meson(self, pkg_config_paths, meson_args):
        # meson_args += ['--cross-file', os.path.join(self.install_folder, 'conan_meson_cross.ini')]

        # does not work. will set CXXFLAGS with all include and link directories
        # TODO issue!
        # self.output.info("pkg_config_paths: %s" % pkg_config_paths)
        # meson.configure( build_folder="meson", defs=meson_options,
        #         args=meson_args,
        #         pkg_config_paths=pkg_config_paths
        #         )
        with_tests = True
        if self.settings.os == "Windows":
            with_tests = False
        with_servers = False
        if self.settings.os == "Linux":
            with_servers = True
        werror = False
        if self.options.treat_warnings_as_errors:
            werror = True
        self.run(['meson', 'setup',
            os.path.join(self.build_folder, 'meson'),
            self.source_folder,
            '--pkg-config-path', ','.join(pkg_config_paths),
            '--prefix', self.package_folder,
            '-Dbackend=ninja',
            '-Dbuildtype=%s' % self._get_meson_build_type(),
            '-Ddefault_library=static',
            '-Dcpp_std=c++17',
            '-Dwerror=%s' % werror,
            '-Db_ndebug=if-release',
            '-Dwith_tests=%s' % with_tests,
            '-Dwith_servers=%s' % with_servers,
            '-Dwith_ui=%s' % self.options.enable_ui
        ] + meson_args)
        self.run(['meson', 'compile',
            '-C', os.path.join(self.build_folder, 'meson'),
        ])


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
