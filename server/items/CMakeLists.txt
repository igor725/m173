add_library(items OBJECT
  item.cpp
  itemDB.cpp
  itemstack.cpp
  toolmaterial.cpp
  list/_impl.cpp
)

target_link_libraries(items PRIVATE spdlog::spdlog)
