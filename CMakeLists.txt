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

find_package(spdlog REQUIRED)
find_package(sockpp REQUIRED)

add_subdirectory(server/network)
add_subdirectory(server/entity)

add_executable(server
	server/entry.cpp
	$<TARGET_OBJECTS:network>
	$<TARGET_OBJECTS:entity>
)

if(WIN32)
  set(platform_libs ws2_32.lib)
endif()

target_link_libraries(server PRIVATE Sockpp::sockpp-static spdlog::spdlog ${platform_libs})
