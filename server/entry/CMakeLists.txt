add_library(entry OBJECT
  entry.cpp
  helper.cpp
)

target_link_libraries(entry PRIVATE spdlog::spdlog)
