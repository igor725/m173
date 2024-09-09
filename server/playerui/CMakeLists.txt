add_subdirectory(windows)

add_library(playerui STATIC
  container.cpp
  slots/_impl.cpp
  containers/playerContainer.cpp
  containers/workbenchContainer.cpp
  containers/furnaceContainer.cpp
  storages/playerStorage.cpp

  $<TARGET_OBJECTS:uiwindows>
)

target_link_libraries(playerui PRIVATE spdlog::spdlog)
