add_library(http_beast
  "client_module.cpp")
target_link_libraries(http_beast PUBLIC fubble::http)
target_link_libraries(http_beast PRIVATE fubble::ssl)
add_library(fubble::http_beast ALIAS http_beast)

