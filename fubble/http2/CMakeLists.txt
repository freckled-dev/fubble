add_library(http2 "module.cpp")
target_link_libraries(http2 PUBLIC fubble::utils nlohmann_json::nlohmann_json)
add_library(fubble::http2 ALIAS http2)

