add_library(items OBJECT
  items.cpp
  itemstack.cpp
)

target_link_libraries(items PRIVATE spdlog::spdlog)
