set(SPRITE_SOURCE_FILES
    src/demo/sprite/main.cpp
    src/demo/sprite/spritedemo.cpp
)

set(SPRITE_TARGET sprite)

add_executable(${SPRITE_TARGET} ${SPRITE_SOURCE_FILES})
target_link_libraries(${SPRITE_TARGET} ${COMMON_TARGET})
