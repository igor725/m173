add_library(mcregion OBJECT
  regionfile.cpp
  mcregion.cpp
)

target_link_libraries(mcregion PRIVATE spdlog::spdlog)
