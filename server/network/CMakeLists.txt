add_library(network OBJECT
  clientloop.cpp
)

target_link_libraries(network PRIVATE Sockpp::sockpp-static spdlog::spdlog)
