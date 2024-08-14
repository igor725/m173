add_library(entity OBJECT
  manager.cpp
  entitybase.cpp
  player/player.cpp
  objects/arrow.cpp
  objects/snowball.cpp
  objects/fishfloat.cpp
)

target_link_libraries(entity PRIVATE spdlog::spdlog)
