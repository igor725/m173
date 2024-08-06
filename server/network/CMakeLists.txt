add_library(network OBJECT
  reader.cpp
)

target_link_libraries(network PRIVATE Sockpp::sockpp-static spdlog::spdlog)
