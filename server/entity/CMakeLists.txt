add_library(entity OBJECT
  manager.cpp
  player/player.cpp
)

target_link_libraries(entity PRIVATE spdlog::spdlog)
