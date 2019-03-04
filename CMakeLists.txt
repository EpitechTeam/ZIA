cmake_minimum_required(VERSION 3.10.2)
project(Zia)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "start running Server cmake...")

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
elseif (UNIX AND APPLE)
    set(APPLE TRUE)
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY .)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wall -O")

include(build/conanbuildinfo.cmake)

include_directories(ZiaApi/lib)
include_directories(Zia/include)

if (LINUX OR APPLE)

    message(STATUS "LINUX")

    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

    conan_basic_setup()

    add_executable(zia
            Zia/src/main.cpp
            Zia/src/Zia.cpp)

    target_link_libraries(zia pthread)
    target_link_libraries(zia ${CONAN_LIBS})
    target_link_libraries(zia ${CMAKE_DL_LIBS})

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

    add_library(ConfigParserModule SHARED
            modules/ConfigParser/ConfigParser.cpp)

    target_link_libraries(ConfigParserModule pthread)
    target_link_libraries(ConfigParserModule ${CONAN_LIBS})

    elseif (WIN32)
        message(STATUS "MAKEFILE DON'T SUPPORT WINDOWS, PLEASE USE VISUAL STUDIO SOLUTION.")

endif ()