set(LUA_DIR ${CMAKE_SOURCE_DIR}/3rd/lua)

add_subdirectory(vm)

add_library(scriptvm STATIC
  $<TARGET_OBJECTS:lua_vm>

  main.cpp
  thread.cpp
  event.cpp
)

target_include_directories(scriptvm PRIVATE ${LUA_DIR})
target_link_libraries(scriptvm PRIVATE spdlog::spdlog)
