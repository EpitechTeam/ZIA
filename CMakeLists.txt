cmake_minimum_required(VERSION 3.10.2)
project(Zia)

add_definitions("-std=c++11")

message(STATUS "start running Server cmake...")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY .)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -g3 -Wall -O")

include_directories(httpServer/include)
include_directories(moduleApi/include)

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
elseif (UNIX AND APPLE)
    set(APPLE TRUE)
endif ()

if (LINUX)
    message(STATUS "LINUX")

    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

    conan_basic_setup()

    add_executable(zia_server
            httpServer/src/Connection.cpp
            httpServer/src/ConnectionManager.cpp
            httpServer/src/main.cpp
            httpServer/src/mime_types.cpp
            httpServer/src/reply.cpp
            httpServer/src/RequestHandler.cpp
            httpServer/src/RequestParser.cpp
            httpServer/src/Server.cpp
            moduleApi/src/moduleManager.cpp
            )

    target_link_libraries(zia_server pthread)
    target_link_libraries(zia_server ${CONAN_LIBS})

    set(CMAKE_CXX_FLAGS "-g3 -Wall -O0")

elseif (APPLE)
    message(STATUS "compiling for macOS")

    #include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(build/conanbuildinfo.cmake)

    conan_basic_setup()

    add_executable(zia_server
            httpServer/src/Connection.cpp
            httpServer/src/ConnectionManager.cpp
            httpServer/src/main.cpp
            httpServer/src/mime_types.cpp
            httpServer/src/reply.cpp
            httpServer/src/RequestHandler.cpp
            httpServer/src/RequestParser.cpp
            httpServer/src/Server.cpp
            moduleApi/src/moduleManager.cpp
            )

    target_link_libraries(zia_server pthread)
    target_link_libraries(zia_server ${CONAN_LIBS})

endif ()

if (WIN32)
    message(STATUS "WINDOWS")
endif ()
