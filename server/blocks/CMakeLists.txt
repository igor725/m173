add_library(blocks OBJECT
  blocks.cpp
)

target_link_libraries(blocks PRIVATE spdlog::spdlog)
