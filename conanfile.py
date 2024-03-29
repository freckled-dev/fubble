from conans import ConanFile, tools
from conans.errors import ConanInvalidConfiguration
from conans.tools import os_info
from conan.tools.cmake import CMakeToolchain, CMake, CMakeDeps
import os

class FubbleConan(ConanFile):
    name = "fubble"
    license = "All Rights Reserved"
    author = "Fubble OG <contact@fubble.io>"
    url = "fubble.io"
    description = "Conferencing that works"
    topics = ("conference", "fubble", "video", "audio", "webrtc")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [False, True],
        "fPIC": [True, False],
        "treat_warnings_as_errors": [True, False],
        "sanatize": [True, False],
        "qt_install": "ANY",
        "enable_ui": [True, False],
        "enable_servers": [True, False],
        }
    # https://docs.conan.io/en/latest/reference/conanfile/attributes.html#default-options
    default_options = {
        "shared": False,
        "fPIC": True,
        "qt_install": None,
        "enable_ui": True,
        "enable_servers": True,
        "restinio:asio": "boost",
        "treat_warnings_as_errors": False,
        "sanatize": False}
    #exports_sources = "*", "!fubble/app/mock_qml_models*", "!.*"
    no_copy_source = True

    # https://docs.conan.io/en/latest/versioning/introduction.html
    version = '1.0.0'

    def _get_qt_bin_paths(self):
        if self.settings.os != "Windows":
            return []
        if self.options.qt_install:
            qt_install = str(self.options.qt_install)
            qt_cmake_dir = os.path.join(qt_install, 'bin')
            return [qt_cmake_dir]
        return []

    def _is_ios(self):
            return self.settings.os == "iOS"

    def _get_build_type(self):
        return self.settings.get_safe("build_type", default="Release")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.sanatize
            # del self.options.enable_servers

    def configure(self):
        if self._get_build_type() == "Release" and 'sanatize' in self.options:
            if self.options.sanatize:
                raise ConanInvalidConfiguration("can't enable sanatize without debug information")

    def imports(self):
        self.copy("*.dll", dst="bin", keep_path=False)

    def build_requirements(self):
        if os_info.is_macos: # maybe even for windows, instead of the pkgconfig-lite "hack"
            self.tool_requires('pkgconf/[>=1.7]')
        if not tools.which('cmake'):
            self.tool_requires('cmake/[>=1.15]')

    def requirements(self):
        self.requires("nlohmann_json/3.7.0")
        self.requires("boost/1.77.0")
        self.requires("sigslot/[>=1.2]")
        self.requires("gtest/1.11.0")
        self.requires("fmt/[>=8.0]")
        self.requires("google-webrtc/94@acof/stable")
        if self.settings.os == "Windows":
            self.requires("openssl/1.1.1l")
        if not self._is_ios() and self.options.enable_ui:
            self.requires("rectanglebinpack/cci.20210901")
        # self.requires("qt/5.15.2")
        self.requires("restinio/0.6.11")

    def generate(self):
        tc = CMakeToolchain(self)
        # TODO refactor to conans qt
        if self.options.qt_install:
            qt_install = str(self.options.qt_install)
            qt_cmake_dir = os.path.join(qt_install, 'lib', 'cmake', 'Qt5')
            self.output.info("qt_cmake_dir: %s" % (qt_cmake_dir))
            tc.variables["Qt5_DIR"] = qt_cmake_dir.replace('\\', '\\\\')
        tc.variables["BUILD_SERVERS"] = self.options.get_safe("enable_servers", default=False)
        tc.variables["BUILD_UI"] = self.options.get_safe("enable_ui", default=False)
        tc.generate()

        cmake = CMakeDeps(self)
        cmake.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build() 

    def package(self):
        cmake = CMake(self)
        cmake.install()

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
            self.cpp_info.system_libs = ['ole32.lib', 'dbgeng.lib']

