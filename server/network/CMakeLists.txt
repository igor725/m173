add_library(network OBJECT
  packets/_impl.cpp
  clientloop.cpp
  safesock.cpp
)

target_link_libraries(network PRIVATE Sockpp::sockpp-static spdlog::spdlog)
