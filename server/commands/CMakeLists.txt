add_library(commands OBJECT
  handler.cpp
  clist.cpp
)

target_link_libraries(commands PRIVATE spdlog::spdlog)
