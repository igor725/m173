add_library(config OBJECT
  config.cpp
)

target_link_libraries(config PRIVATE spdlog::spdlog)
