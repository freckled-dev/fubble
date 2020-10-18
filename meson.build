project('fubble', 'cpp',
  meson_version : '>=0.55.0',
  license: ['proprietary'],
  default_options : ['cpp_std=c++17'])
# there is a bug in meson 0.52.0. cant use boost with meson
# https://github.com/mesonbuild/meson/issues/6003

disabled = disabler()
if get_option('with_servers')
  enable_servers = declare_dependency()
else
  enable_servers = disabled
endif

is_debug = get_option('debug')

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
boost_compile_args = ['-DBOOST_THREAD_VERSION=5',
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
  cxx = meson.get_compiler('cpp')
  if cxx.has_header('backtrace.h')
    message('using boost_stacktrace_backtrace')
    boost_dep_modules += [
      'stacktrace_backtrace',
      ]
    boost_dep_link_with += [
      '-lbacktrace',
      ]
  else
    # message('using boost_stacktrace_basic')
    message('using boost_stacktrace_addr2line')
    boost_dep_modules += [
      # 'stacktrace_basic',
      'stacktrace_addr2line',
      ]
  endif
  boost_dep_link_with += [
    '-rdynamic',
    '-no-pie'
    ]
  boost_compile_args += [
    '-DBOOST_STACKTRACE_LINK',
    '-rdynamic',
    # if "-no-pie" is not set, stacktrace cant load crash on restart.
    # because of "address space randomization".
    # https://stackoverflow.com/questions/47778099/what-is-no-pie-used-for
    # when product is major enough, remove it
    '-no-pie'
    ]
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
fruit_dep = dependency('fruit', include_type: 'system')
json_dep = dependency('nlohmann_json')
gstreamer_webrtc_dep_pkg = dependency('gstreamer-webrtc-1.0',
  required : false, disabler : true)
gstreamer_sdp_dep = dependency('gstreamer-sdp-1.0',
  required : false, disabler : true)
qt5 = import('qt5')
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
  google_webrtc_system_deps = declare_dependency(
    dependencies: [x11_dep, xext_dep, xcomposite_dep, xfixes_dep,
      xdamage_dep, xrandr_dep]
    )
else
  google_webrtc_system_deps = declare_dependency(
    link_args: ['d3d11.lib', 'dxgi.lib']
  )
endif
google_webrtc_dep_fixme = dependency('google-webrtc', include_type : 'system')
google_webrtc_dep = declare_dependency(
  dependencies: [google_webrtc_dep_fixme, google_webrtc_system_deps]
  )
# ssl_dep = dependency('openssl', include_type: 'system', required: false)
ssl_dep = google_webrtc_dep
rectangle_bin_pack_dep = dependency('RectangleBinPack', include_type: 'system')
if host_machine.system() == 'linux'
  restinio_dep = dependency('restinio', include_type: 'system')
else
  restinio_dep = disabled
endif

subdir('asio')
subdir('utils')
subdir('http')
subdir('websocket')
subdir('matrix')
subdir('signalling')
subdir('webrtc_peer')
subdir('temporary_room')
subdir('version')
subdir('client')

