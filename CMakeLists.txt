cmake_minimum_required(VERSION 3.10.2)
project(R-Type)

set(CMAKE_CXX_STANDARD 14)

message(STATUS "start running Server cmake...")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY .)

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (LINUX)
    message(STATUS "LINUX")

    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

    conan_basic_setup()

    include_directories(server/include)

    add_executable(zia_server
            server/sources/main.cpp
            )

    target_link_libraries(zia_server pthread)
    target_link_libraries(zia_server ${CONAN_LIBS})

    set(CMAKE_CXX_FLAGS "-g3 -Wall -O0 -std=c++11")

endif ()

if (WIN32)
    message(STATUS "WINDOWS")
endif ()