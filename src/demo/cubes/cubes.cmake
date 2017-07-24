set(CUBES_SOURCE_FILES
    src/demo/cubes/main.cpp
    src/demo/cubes/cubesdemo.cpp
)


find_package(Bullet)
if(NOT ${BULLET_FOUND})
    message("skipping cubes")
else()
    set(CUBES_TARGET cubes)

    add_executable(${CUBES_TARGET} ${CUBES_SOURCE_FILES})
    target_link_libraries(${CUBES_TARGET} ${COMMON_TARGET})
    target_link_libraries(${CUBES_TARGET} ${BULLET_LIBRARIES})

    target_include_directories(${CUBES_TARGET} PUBLIC ${BULLET_INCLUDE_DIRS})
endif()
