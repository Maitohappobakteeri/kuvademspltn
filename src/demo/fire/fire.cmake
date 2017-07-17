set(FIRE_SOURCE_FILES
    src/demo/fire/main.cpp
)

set(FIRE_TARGET fire)

add_executable(${FIRE_TARGET} ${FIRE_SOURCE_FILES})
target_link_libraries(${FIRE_TARGET} ${COMMON_TARGET})
