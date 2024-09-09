add_subdirectory(mcregion)

add_library(world STATIC
  world.cpp
  chunkCompression.cpp
  generators/glist.cpp
  generators/list/flat.cpp
  $<TARGET_OBJECTS:mcregion>
)
