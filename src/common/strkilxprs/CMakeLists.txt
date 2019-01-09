cmake_minimum_required(VERSION 3.7.2)
project(strkilxprs)

set(PROJECT_NAME ${PROJECT_NAME})
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/project.hpp.in"
  "${PROJECT_BINARY_DIR}/project.hpp"
  )

set(LIB_SOURCE_FILES
    src/strkilxprs.cpp
    src/object.cpp
    src/map.cpp
    src/list.cpp
    src/string.cpp
    src/plain.cpp
    src/parse.cpp
    src/error.cpp
)

set(LIB_HEADER_FILES
    src/strkilxprs.hpp
    src/object.hpp
    src/map.hpp
    src/list.hpp
    src/string.hpp
    src/plain.hpp
    src/parse.hpp
    src/error.hpp
)

set(TEST_SOURCE_FILES
    src/test/main.cpp
)

include_directories("src")
include_directories("${PROJECT_BINARY_DIR}")

find_program(GPP_COMPILER g++)
if(NOT GPP_COMPILER)
    message(WARNING "g++ compiler not found, skipping warning and error options")
else()
    set(CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror -Wfatal-errors -Wno-unused-function"
    )
    set(CMAKE_CXX_COMPILER ${GPP_COMPILER})
endif()

set(LIB_TARGET ${PROJECT_NAME})
set(TEST_TARGET test${PROJECT_NAME})

add_library(${LIB_TARGET} SHARED ${LIB_SOURCE_FILES})

add_executable(${TEST_TARGET} ${TEST_SOURCE_FILES})
target_link_libraries(${TEST_TARGET} ${LIB_TARGET})

install(TARGETS ${LIB_TARGET}
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib
      PUBLIC_HEADER DESTINATION include
)

INSTALL(FILES ${LIB_HEADER_FILES}
        DESTINATION include/${PROJECT_NAME}
)
