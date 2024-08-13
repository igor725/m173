add_library(blocks OBJECT
  block.cpp
  blockDB.cpp
)

target_link_libraries(blocks PRIVATE spdlog::spdlog)
