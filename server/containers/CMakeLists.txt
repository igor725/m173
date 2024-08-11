add_library(containers OBJECT
  container.cpp
  list/playerContainer.cpp
)

target_link_libraries(containers PRIVATE spdlog::spdlog)
