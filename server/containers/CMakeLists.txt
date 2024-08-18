add_library(containers OBJECT
  container.cpp
  slots/_impl.cpp
  list/playerContainer.cpp
  list/workbenchContainer.cpp
  storages/playerStorage.cpp
)

target_link_libraries(containers PRIVATE spdlog::spdlog)
