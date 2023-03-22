set(CUBES_SOURCE_FILES
    src/demo/cubes/main.cpp
    src/demo/cubes/cubesdemo.cpp
)


set(CUBES_TARGET cubes)

add_executable(${CUBES_TARGET} ${CUBES_SOURCE_FILES})
target_link_libraries(${CUBES_TARGET} ${COMMON_TARGET})
target_link_libraries(${CUBES_TARGET} bullet)

target_include_directories(${CUBES_TARGET} PUBLIC "/home/jenna/emsdk/upstream/emscripten/cache/sysroot/include/bullet/")
