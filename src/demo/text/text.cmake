set(TEXT_SOURCE_FILES
    src/demo/text/main.cpp
)

set(TEXT_TARGET text)

add_executable(${TEXT_TARGET} ${TEXT_SOURCE_FILES})
target_link_libraries(${TEXT_TARGET} ${COMMON_TARGET})
