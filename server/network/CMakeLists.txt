add_library(network OBJECT
  packets/${M173_PROTO_VERSION}/_impl.cpp
  packets/_impl.cpp
  clientloop.cpp
  bmanager.cpp
  safesock.cpp
)

target_link_libraries(network PRIVATE Sockpp::sockpp-static spdlog::spdlog)
