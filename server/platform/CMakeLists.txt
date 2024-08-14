if(WIN32)
  set(PLAT_FILE "windows.cpp")
else()
  set(PLAT_FILE "unix.cpp")
endif()

add_library(platform OBJECT
  ${PLAT_FILE}
)

# target_link_libraries(platform PRIVATE spdlog::spdlog)
