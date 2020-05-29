http_sources = ['client.cpp', 'action.cpp', 'connection_creator.cpp', 
    'action_factory.cpp', 'connection_impl.cpp', 'ssl_upgrader.cpp']
http_dependencies = [utils_dep, fmt_dep, json_dep]
if host_machine.system() == 'windows'
  http_sources += 'add_windows_root_certs.cpp'
  crypt32_dep = meson.get_compiler('cpp').find_library('crypt32')
  http_dependencies += crypt32_dep
endif
http = library('http',
  http_sources,
  dependencies: http_dependencies,
  include_directories: ['..'],
  cpp_pch: '../pch/precompiled.hpp',
  install: true
  )
http_dep = declare_dependency(
  include_directories: '..',
  link_with: [http],
  dependencies: [utils_dep, json_dep]
  )
http_test = executable('http_test',
  ['client_test.cpp'],
  dependencies : [gtest_dep, http_dep, fmt_dep],
  cpp_pch: '../pch/precompiled.hpp',
  link_with: [test_main]
  )

test('http test',
  http_test,
  env: [ 'ASAN_OPTIONS=fast_unwind_on_malloc=0' ]
  )
