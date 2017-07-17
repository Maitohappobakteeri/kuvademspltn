cmake_minimum_required(VERSION 3.7.2)
project(kuvademspltn)

set(PROJECT_NAME ${PROJECT_NAME})
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/common/project.hpp.in"
  "${PROJECT_BINARY_DIR}/project.hpp"
  )

include_directories("src")
include_directories("${PROJECT_BINARY_DIR}")

find_program(GPP_COMPILER g++)
if(NOT GPP_COMPILER)
    message(WARNING "g++ compiler not found, skipping some compiler settings")
else()
    set(CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror -Wfatal-errors -Wno-unused-function"
    )
    set(CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -O3"
    )
    set(CMAKE_CXX_COMPILER ${GPP_COMPILER})
endif()

find_package(PNG)
find_package(GLEW)
find_package(OpenGL)
find_package(Boost)
find_package(Freetype)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

INCLUDE(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX("glm/glm.hpp" HAS_GLM)

IF(NOT HAS_GLM)
  UNSET(HAS_GLM CACHE)
  message(FATAL_ERROR "glm/glm.hpp not found" )
ENDIF()

if(NOT ${PNG_FOUND})
    message(FATAL_ERROR)
endif()
if(NOT ${GLEW_FOUND})
    message(FATAL_ERROR)
endif()
if(NOT ${OpenGL_FOUND})
    message(FATAL_ERROR)
endif()
if(NOT ${SDL2_FOUND})
    message(FATAL_ERROR)
endif()
if(NOT ${Boost_FOUND})
    message(FATAL_ERROR)
endif()
if(NOT ${Freetype_FOUND})
    message(FATAL_ERROR)
endif()

link_directories(${CMAKE_BINARY_DIR}/lib)
include_directories(${CMAKE_BINARY_DIR}/include)

include(ExternalProject)
ExternalProject_Add(prs
   SOURCE_DIR ${CMAKE_SOURCE_DIR}/strkilxprs/
   INSTALL_DIR ${CMAKE_BINARY_DIR}/
   CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/
   BUILD_ALWAYS 0
)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIR_freetype2} ${FREETYPE_INCLUDE_DIR_ft2build})

set(COMMON_SOURCE_FILES
    src/common/demo.cpp
    src/common/window.cpp
    src/common/xwindow.cpp
    src/common/loadsprite.cpp
    src/common/print.cpp
    src/common/residgenerator.cpp
    src/common/rng.cpp

    src/common/renderer/renderer.cpp
    src/common/renderer/shader.cpp
    src/common/renderer/texture.cpp
    src/common/renderer/buffer.cpp
    src/common/renderer/framebuffer.cpp
    src/common/renderer/dynamicbox.cpp
    src/common/renderer/sprite.cpp
    src/common/renderer/spritegroup.cpp
    src/common/renderer/font.cpp
    src/common/renderer/text.cpp
)

set(COMMON_TARGET ${PROJECT_NAME})

add_library(${COMMON_TARGET} SHARED ${COMMON_SOURCE_FILES})
add_dependencies(${COMMON_TARGET} prs)

target_include_directories(${COMMON_TARGET} PUBLIC src/common/)

target_link_libraries(${COMMON_TARGET} ${SDL2_LIBRARIES})
target_link_libraries(${COMMON_TARGET} ${PNG_LIBRARIES})
target_link_libraries(${COMMON_TARGET} ${GLEW_LIBRARIES})
target_link_libraries(${COMMON_TARGET} ${OPENGL_gl_LIBRARY})
target_link_libraries(${COMMON_TARGET} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${COMMON_TARGET} ${FREETYPE_LIBRARIES})
target_link_libraries(${COMMON_TARGET} strkilxprs)

include(src/demo/text/text.cmake)
include(src/demo/sprite/sprite.cmake)
include(src/demo/fire/fire.cmake)
