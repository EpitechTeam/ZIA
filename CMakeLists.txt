cmake_minimum_required(VERSION 3.10.2)
project(Zia)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "start running Server cmake...")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY .)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -g3 -Wall -O")

include_directories(ZiaApi/lib)
include_directories(Zia/include)

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
elseif (UNIX AND APPLE)
    set(APPLE TRUE)
endif ()

if (LINUX)
    message(STATUS "LINUX")

    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

    conan_basic_setup()

    add_executable(zia
            Zia/src/main.cpp
            Zia/src/Zia.cpp)

    target_link_libraries(zia pthread)
    target_link_libraries(zia ${CONAN_LIBS})


    add_library(ConnectionModule SHARED
            modules/Connection/ConnectionModule.cpp)

    target_link_libraries(ConnectionModule pthread)
    target_link_libraries(ConnectionModule ${CONAN_LIBS})


    add_library(HttpServerModule SHARED
            modules/HttpServer/HttpServer.cpp)

    target_link_libraries(HttpServerModule pthread)
    target_link_libraries(HttpServerModule ${CONAN_LIBS})

    add_library(ParamsModule SHARED
            modules/Params/Params.cpp)

    target_link_libraries(ParamsModule pthread)
    target_link_libraries(ParamsModule ${CONAN_LIBS})

    add_library(PhpCgiModule SHARED
            modules/PhpCgi/PhpCgi.cpp)

    target_link_libraries(PhpCgiModule pthread)
    target_link_libraries(PhpCgiModule ${CONAN_LIBS})

    set(CMAKE_CXX_FLAGS "-g3 -Wall -O0")

elseif (APPLE)
    message(STATUS "compiling for macOS")

    include(build/conanbuildinfo.cmake)
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

    target_link_libraries(zia pthread)
    target_link_libraries(zia ${CONAN_LIBS})

endif ()

if (WIN32)
    message(STATUS "WINDOWS")
endif ()

target_link_libraries(zia ${CMAKE_DL_LIBS})