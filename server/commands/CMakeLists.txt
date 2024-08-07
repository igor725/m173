add_library(commands OBJECT
  command.cpp
)

target_link_libraries(commands PRIVATE spdlog::spdlog)
