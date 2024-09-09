add_library(mcnetwork STATIC
  packets/${M173_PROTO_VERSION}_impl.cpp
  packets/0_impl.cpp
  clientloop.cpp
  bmanager.cpp
  safesock.cpp
  protover.cpp
)

if(M173_BETA18_PROTO)
  target_compile_options(mcnetwork PRIVATE -DM173_BETA18_PROTO)
endif()

target_link_libraries(mcnetwork PRIVATE Sockpp::sockpp-static spdlog::spdlog)
