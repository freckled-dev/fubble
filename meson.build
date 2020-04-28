project('fubble', 'cpp',
  meson_version : '>=0.53.0',
  default_options : ['cpp_std=c++17'])
# there is a bug in meson 0.52.0. cant use boost with meson
# https://github.com/mesonbuild/meson/issues/6003
# TODO currently in >=0.54 boost doesn't work either

disabled = disabler()
if get_option('with_servers')
  enable_servers = declare_dependency()
else
  enable_servers = disabled
endif

if host_machine.system() == 'windows'
  # support windows 7 onwards
  add_project_arguments('-D_WIN32_WINNT=0x0601', language: 'cpp')
  # https://stackoverflow.com/questions/39689162/qt-project-in-visual-studio-2015-unresolved-external-symbol-wwinmain
  # TODO shall be only disabled on release
  add_project_link_arguments('/entry:mainCRTStartup', language: 'cpp')
endif

boost_dep_pkg = dependency('boost', modules : [
    'thread', 'program_options', 'regex', 'log', 'log_setup', 'filesystem'
    ], static: true, include_type : 'system'
)
boost_dep_link_with = []
if host_machine.system() == 'windows'
  boost_dep_link_with = ['bcrypt.lib']
endif
boost_dep = declare_dependency(
    compile_args : ['-DBOOST_THREAD_VERSION=5'
      # , '-DBOOST_LOG_DYN_LINK'
      ],
    dependencies : [boost_dep_pkg],
    link_args: boost_dep_link_with
    )
if get_option('with_tests')
  gtest_dep = dependency('gtest', include_type : 'system')
else
  gtest_dep = disabled
endif
fmt_dep = dependency('fmt')
json_dep = dependency('nlohmann_json')
boost_di_dep = dependency('boost-di')
gstreamer_webrtc_dep_pkg = dependency('gstreamer-webrtc-1.0',
  required : false, disabler : true)
gstreamer_sdp_dep = dependency('gstreamer-sdp-1.0',
  required : false, disabler : true)
qt5 = import('qt5')
qt5_clear_dep = dependency('qt5', modules: ['Core', 'Gui', 'Qml', 'Quick', 'Multimedia', 'Svg', 'QuickControls2'])
# `QT_NO_EMIT` is neccessary because google webrtc declares a method/function called `emit`
qt5_dep = declare_dependency(
  compile_args : ['-DQT_DEPRECATED_WARNINGS', '-DQT_QML_DEBUG=1', '-DQT_NO_EMIT'],
  dependencies : [qt5_clear_dep]
  )
gstreamer_webrtc_dep = declare_dependency(
  dependencies : [gstreamer_webrtc_dep_pkg, gstreamer_sdp_dep,
    gstreamer_webrtc_dep_pkg
    ]
  )
if host_machine.system() == 'linux'
  x11_dep = dependency('x11')
else
  x11_dep = declare_dependency()
endif
google_webrtc_dep_fixme = dependency('google-webrtc', include_type : 'system')
google_webrtc_dep = declare_dependency(
  dependencies: [google_webrtc_dep_fixme, x11_dep]
  )

subdir('utils')
subdir('http')
subdir('websocket')
subdir('matrix')
subdir('signalling')
subdir('webrtc_peer')
subdir('temporary_room')
subdir('session')
subdir('client')

