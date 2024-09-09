add_library(config STATIC
  config.cpp
)

target_link_libraries(config PRIVATE spdlog::spdlog)
