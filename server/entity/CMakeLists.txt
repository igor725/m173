add_library(entity OBJECT
  manager.cpp
  entitybase.cpp
  creaturebase.cpp
  creatures/player.cpp
  objects/arrow.cpp
  objects/snowball.cpp
  objects/fishfloat.cpp
  objects/thunderbolt.cpp
)

target_link_libraries(entity PRIVATE spdlog::spdlog)
