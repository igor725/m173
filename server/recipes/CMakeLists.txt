add_library(recipes OBJECT
  crafting/recipe.cpp
  crafting/database.cpp
  smelting/database.cpp
)

target_link_libraries(recipes PRIVATE spdlog::spdlog)

