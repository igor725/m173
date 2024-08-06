add_subdirectory(events/world)

add_library(eventsystem OBJECT
  $<TARGET_OBJECTS:events_world>
)
