set(ROISKE_SOURCE_FILES
    src/demo/roiske/main.cpp
    src/demo/roiske/roiskedemo.cpp
)

set(ROISKE_TARGET roiske)

add_executable(${ROISKE_TARGET} ${ROISKE_SOURCE_FILES})
target_link_libraries(${ROISKE_TARGET} ${COMMON_TARGET})
