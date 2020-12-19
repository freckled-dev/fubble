project('fubble', 'cpp',
  meson_version : '>=0.56.0',
  license: ['proprietary'],
  default_options : ['cpp_std=c++17'])

add_project_arguments('-DBUILDING_FUBBLE', language: 'cpp')

disabled = disabler()
if get_option('with_servers')
  enable_servers = declare_dependency()
else
  enable_servers = disabled
endif

cxx = meson.get_compiler('cpp')

# debug PATH
# error(run_command('sh', '-c', 'echo $PATH').stdout().strip())

is_debug = get_option('debug')

is_ios = false
if host_machine.system() == 'darwin'
  ios_test_code = '''#include <TargetConditionals.h>
  #if ! TARGET_OS_IPHONE
  #error "Not iOS/tvOS/watchOS/iPhoneSimulator"
  #endif'''
  if cxx.compiles(ios_test_code, name : 'building for iOS')
    is_ios = true
  endif
endif


if host_machine.system() == 'windows'
  # support windows 7 onwards
  add_project_arguments('-D_WIN32_WINNT=0x0601', language: 'cpp')
  # https://stackoverflow.com/questions/39689162/qt-project-in-visual-studio-2015-unresolved-external-symbol-wwinmain
  # TODO shall be only disabled on release
  add_project_link_arguments('/entry:mainCRTStartup', language: 'cpp')
  if is_debug
    add_project_arguments('/bigobj', language: 'cpp')
  endif
endif

boost_dep_modules = ['thread', 'program_options', 'regex', 'log', 'log_setup',
  'filesystem']
boost_dep_link_with = []
boost_compile_args = [
  '-DBOOST_THREAD_VERSION=5',
  '-DBOOST_ASIO_SEPARATE_COMPILATION',
  '-DBOOST_BEAST_SEPARATE_COMPILATION',
  ]
if host_machine.system() == 'windows'
  boost_dep_link_with += ['bcrypt.lib']
  boost_dep_modules += [
    'stacktrace_windbg',
    ]
  boost_compile_args += [
    '-DBOOST_STACKTRACE_LINK',
    ]
  boost_dep_link_with += [
    'ole32.lib',
    'dbgeng.lib',
    ]
else
  if cxx.has_header('backtrace.h')
    message('using boost_stacktrace_backtrace')
    boost_dep_modules += [
      'stacktrace_backtrace',
      ]
    boost_dep_link_with += [
      '-lbacktrace',
      ]
  elif dependency('boost', modules : ['stacktrace_addr2line'], static: true, required: false).found()
    message('using boost_stacktrace_addr2line')
    boost_dep_modules += [
      'stacktrace_addr2line',
      ]
  else
    message('using boost_stacktrace_basic')
    boost_dep_modules += [
       'stacktrace_basic',
      ]
  endif
  boost_compile_args += [
    '-DBOOST_STACKTRACE_LINK',
    ]
  if host_machine.system() == 'linux'
    boost_compile_args += [
      '-rdynamic',
      # if "-no-pie" is not set, stacktrace cant load crash on restart.
      # because of "address space randomization".
      # https://stackoverflow.com/questions/47778099/what-is-no-pie-used-for
      # when product is major enough, remove it
      '-no-pie',
      ]
    boost_dep_link_with += [
      '-rdynamic',
      '-no-pie'
      ]
  endif
  if host_machine.system() == 'darwin'
    boost_compile_args += [
      '-fexceptions',
      # '-fvisibility=default',
      ]
  endif
  if is_ios
    boost_compile_args += [
      '-DBOOST_AC_USE_PTHREADS',
      '-DBOOST_SP_USE_PTHREADS'
      ]
  endif
endif
boost_dep_pkg = dependency('boost', modules : boost_dep_modules, static: true,
  include_type : 'system'
  )
boost_dep = declare_dependency(
    compile_args : boost_compile_args,
    dependencies : [boost_dep_pkg],
    link_args: boost_dep_link_with
    )
if get_option('with_tests')
  gtest_dep = dependency('gtest', include_type : 'system')
else
  gtest_dep = disabled
endif
fmt_dep = dependency('fmt', include_type: 'system')
json_dep = dependency('nlohmann_json', include_type: 'system')
gstreamer_webrtc_dep_pkg = dependency('gstreamer-webrtc-1.0',
  required : false, disabler : true)
gstreamer_sdp_dep = dependency('gstreamer-sdp-1.0',
  required : false, disabler : true)
if get_option('with_ui')
  qt5_clear_dep = dependency('qt5',
    modules: ['Core', 'Gui', 'Qml', 'Quick', 'Widgets', 'Network', 'Multimedia',
      'Svg', 'QuickControls2', 'Charts'], method: 'qmake',
    version: '>=5.15.0')
  qt5_dep_dependencies = [qt5_clear_dep]
  qt5_dep_link_with = []
  if host_machine.system() == 'windows' and false
    # needed because of static linking
    # dependencies are from qt.pc
    qt_pc_depends = ['zlib',
      # 'openssl',
      'libpcre2', 'glib', 'double-conversion', 'freetype2',
      'harfbuzz', 'libjpeg', 'libpng', 'sqlite3', 'libpq', 'openal', 'zstd', 'opengl']
    foreach dependency : qt_pc_depends
      qt5_dep_dependencies += dependency(dependency)
    endforeach
    qt5_dep_link_with += []
  endif
  # `QT_NO_EMIT` is neccessary because google webrtc declares a method/function called `emit`
  qt5_dep_compile_args = ['-DQT_DEPRECATED_WARNINGS', '-DQT_NO_EMIT']
  if is_debug
    qt5_dep_compile_args += '-DQT_QML_DEBUG=1'
    message('enabled QT_QML_DEBUG')
  else
    message('disabled QT_QML_DEBUG')
  endif
  qt5_dep = declare_dependency(
    compile_args: qt5_dep_compile_args,
    dependencies: qt5_dep_dependencies,
    link_with: qt5_dep_link_with
    )
  qt5 = import('qt5')
else
  qt5_dep = disabled
endif
gstreamer_webrtc_dep = declare_dependency(
  dependencies : [gstreamer_webrtc_dep_pkg, gstreamer_sdp_dep,
    gstreamer_webrtc_dep_pkg
    ]
  )
if host_machine.system() == 'linux'
  x11_dep = dependency('x11')
  xext_dep = dependency('xext')
  xcomposite_dep = dependency('xcomposite')
  xfixes_dep = dependency('xfixes')
  xdamage_dep = dependency('xdamage')
  xrandr_dep = dependency('xrandr')
  #glib_dep = dependency('glib-2.0')
  #glib_dbus_dep = dependency('dbus-glib-1')
  gio_dep = dependency('gio-2.0')
  google_webrtc_system_deps = declare_dependency(
    dependencies: [x11_dep, xext_dep, xcomposite_dep, xfixes_dep,
      xdamage_dep, xrandr_dep,
      #glib_dep,
      #glib_dbus_dep,
      gio_dep,
      ]
    )
elif host_machine.system() == 'windows'
  google_webrtc_system_deps = declare_dependency(
    link_args: ['d3d11.lib', 'dxgi.lib']
  )
elif host_machine.system() == 'darwin'
  if is_ios
    webrtc_apple_deps = dependency('appleframeworks', modules : [
      'audiotoolbox',
      'avfoundation',
      'coreaudio',
      'foundation',
      'uikit',
      'coremedia',
      ])
  else
    webrtc_apple_deps = dependency('appleframeworks', modules : [
      'audiotoolbox', # maybe not needed
      'cocoa',
      'coreaudio',
      'foundation',
      'iosurface',
      ])
  endif
  google_webrtc_system_deps = declare_dependency(
    dependencies: [webrtc_apple_deps]
    )
else
  error('unsupported target')
endif
google_webrtc_dep_fixme = dependency('google-webrtc', include_type : 'system')
google_webrtc_dep = declare_dependency(
  dependencies: [google_webrtc_dep_fixme, google_webrtc_system_deps]
  )
# ssl_dep = dependency('openssl', include_type: 'system', required: false)
ssl_dep = google_webrtc_dep
rectangle_bin_pack_dep = dependency('RectangleBinPack', include_type: 'system', required: false, disabler: true)
if host_machine.system() == 'linux'
  restinio_dep = dependency('restinio', include_type: 'system')
else
  restinio_dep = disabled
endif

subdir('fubble')

