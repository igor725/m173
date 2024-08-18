add_library(blocks OBJECT
  block.cpp
  blockDB.cpp
  list/_impl.cpp
)

target_link_libraries(blocks PRIVATE spdlog::spdlog)
