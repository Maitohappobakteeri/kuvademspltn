set(SIGNAALI_SOURCE_FILES
    src/demo/signaali/main.cpp
    src/demo/signaali/signaalidemo.cpp
)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(PULSE libpulse-simple)

find_package(Threads)

if(Threads_FOUND AND PULSE_FOUND)
    set(SIGNAALI_TARGET signaali)

    add_executable(${SIGNAALI_TARGET} ${SIGNAALI_SOURCE_FILES})
    target_link_libraries(${SIGNAALI_TARGET} ${COMMON_TARGET})
    target_link_libraries(${SIGNAALI_TARGET} ${PULSE_LIBRARIES})
    target_link_libraries(${SIGNAALI_TARGET} ${CMAKE_THREAD_LIBS_INIT})

    target_include_directories(${SIGNAALI_TARGET} PUBLIC ${PULSE_INCLUDE_DIRS})
else()
    message(WARNING "skipping signaali")
endif()
