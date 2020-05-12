asio = library('asio',
  ['asio_impl.cpp'],
  dependencies: [boost_dep, ssl_dep],
  install: true
  )
asio_dep = declare_dependency(
  link_with: [asio],
  dependencies: [boost_dep]
  )

