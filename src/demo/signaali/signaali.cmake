set(SIGNAALI_SOURCE_FILES
    src/demo/signaali/main.cpp
    src/demo/signaali/signaalidemo.cpp
)

set(SIGNAALI_TARGET signaali)

add_executable(${SIGNAALI_TARGET} ${SIGNAALI_SOURCE_FILES})
target_link_libraries(${SIGNAALI_TARGET} ${COMMON_TARGET})
