if(WIN32)
  set(PLAT_FILE "windows.cpp")

  set(platform_libs ws2_32.lib $<$<CONFIG:Debug>:zlibstaticd>$<$<CONFIG:Release>:zlibstatic>.lib)
else()
  set(PLAT_FILE "unix.cpp")


  set(platform_libs z.a)
endif()

add_library(platform STATIC
  ${PLAT_FILE}
)

target_link_libraries(platform PRIVATE spdlog::spdlog ${platform_libs})
