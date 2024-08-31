cmake_minimum_required(VERSION 3.24)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
set(CMAKE_WINDOWS_SYMBOL_VISIBILITY_IN_STATIC_LIBRARIES OFF)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

project(m173 VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INSTALL_MESSAGE LAZY)
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/3rd/install/lib/cmake)

option(M173_COPY_SCRIPTS "This option enables copying of default Lua scripts to installation directory" ON)
option(M173_BETA18_PROTO "This option specifies used protocol version during compilation" OFF)

include_directories(BEFORE
  ${CMAKE_SOURCE_DIR}/3rd/install/include
  ${CMAKE_SOURCE_DIR}/server
)

link_directories(BEFORE
  ${CMAKE_SOURCE_DIR}/3rd/install/lib
)

find_package(spdlog "1.14.1" REQUIRED)
find_package(sockpp "1.0.0" REQUIRED)

if(M173_BETA18_PROTO)
  set(M173_PROTO_VERSION "17")
  add_definitions(-DM173_BETA18_PROTO)
else()
  set(M173_PROTO_VERSION "14")
endif()

if(WIN32)
  set(platform_defines "-DNOMINMAX -DWIN32_LEAN_AND_MEAN")
endif()

add_definitions(${platform_defines} -DSPDLOG_USE_STD_FORMAT)

add_subdirectory(server)

add_executable(m173_server
  $<TARGET_OBJECTS:entry>
  $<TARGET_OBJECTS:config>
  $<TARGET_OBJECTS:commands>
  $<TARGET_OBJECTS:network>
  $<TARGET_OBJECTS:blocks>
  $<TARGET_OBJECTS:items>
  $<TARGET_OBJECTS:recipes>
  $<TARGET_OBJECTS:entity>
  $<TARGET_OBJECTS:containers>
  $<TARGET_OBJECTS:mcregion>
  $<TARGET_OBJECTS:world>
  $<TARGET_OBJECTS:zlibpp>
  $<TARGET_OBJECTS:platform>
  $<TARGET_OBJECTS:runmanager>
)

if(WIN32)
  set(platform_libs ws2_32.lib $<$<CONFIG:Debug>:zlibstaticd>$<$<CONFIG:Release>:zlibstatic>.lib)
else()
  set(platform_libs z.a)
endif()

target_link_libraries(m173_server PRIVATE Sockpp::sockpp-static spdlog::spdlog scriptvm ${platform_libs})

install(
  TARGETS m173_server
  DESTINATION ${CMAKE_INSTALL_PREFIX}
)

if(WIN32)
  install(
    FILES $<TARGET_PDB_FILE:m173_server>
    DESTINATION ${CMAKE_INSTALL_PREFIX} OPTIONAL
  )
endif()

if(M173_COPY_SCRIPTS)
install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/scripts
  DESTINATION ${CMAKE_INSTALL_PREFIX}
)
endif()
