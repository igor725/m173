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

include_directories(BEFORE
  ${CMAKE_SOURCE_DIR}/3rd/install/include
  ${CMAKE_SOURCE_DIR}/server
)

link_directories(BEFORE
  ${CMAKE_SOURCE_DIR}/3rd/install/lib
)

find_package(spdlog "1.14.1" REQUIRED)
find_package(sockpp "1.0.0" REQUIRED)

add_definitions(-DNOMINMAX -DWIN32_LEAN_AND_MEAN)

add_subdirectory(server/config)
add_subdirectory(server/commands)
add_subdirectory(server/network)
add_subdirectory(server/blocks)
add_subdirectory(server/items)
add_subdirectory(server/entity)
add_subdirectory(server/containers)
add_subdirectory(server/world)
add_subdirectory(server/zlibpp)

add_executable(m173_server
	server/entry.cpp
	$<TARGET_OBJECTS:config>
	$<TARGET_OBJECTS:commands>
	$<TARGET_OBJECTS:network>
	$<TARGET_OBJECTS:blocks>
	$<TARGET_OBJECTS:items>
	$<TARGET_OBJECTS:entity>
	$<TARGET_OBJECTS:containers>
	$<TARGET_OBJECTS:world>
	$<TARGET_OBJECTS:zlibpp>
)

if(WIN32)
  set(platform_libs ws2_32.lib $<$<CONFIG:Debug>:zlibstaticd>$<$<CONFIG:Release>:zlibstatic>.lib)
else()
  set(platform_libs z)
endif()

target_link_libraries(m173_server PRIVATE Sockpp::sockpp-static spdlog::spdlog ${platform_libs})

set(InstallDir ${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE})

install(TARGETS m173_server
  COMPONENT m173_server
  RUNTIME_DEPENDENCIES
  PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
  POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"

  DIRECTORIES
  $<TARGET_FILE_NAME:m173_server>
  DESTINATION ${InstallDir}
)

install(FILES $<TARGET_PDB_FILE:m173_server> DESTINATION ${InstallDir} OPTIONAL)
