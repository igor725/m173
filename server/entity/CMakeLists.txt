add_library(entity STATIC
  manager.cpp
  player/player.cpp
)

target_link_libraries(entity PRIVATE spdlog::spdlog)
