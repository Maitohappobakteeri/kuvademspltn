set(DISINTERGRATE_SOURCE_FILES
    src/demo/disintegrate/main.cpp
    src/demo/disintegrate/disintdemo.cpp
)

set(DISINTERGRATE_TARGET disintegrate)

add_executable(${DISINTERGRATE_TARGET} ${DISINTERGRATE_SOURCE_FILES})
target_link_libraries(${DISINTERGRATE_TARGET} ${COMMON_TARGET})
