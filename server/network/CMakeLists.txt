add_library(network STATIC
  reader.cpp
)

target_link_libraries(network PRIVATE Sockpp::sockpp-static spdlog::spdlog)
