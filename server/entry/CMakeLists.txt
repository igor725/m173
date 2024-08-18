add_library(entry OBJECT
  entry.cpp
)

target_link_libraries(entry PRIVATE spdlog::spdlog)
