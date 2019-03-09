cmake_minimum_required(VERSION 3.10.2)
project(Zia)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "start running Server cmake...")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY .)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wall -O")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup()

include_directories(ZiaApi/lib)
include_directories(Zia/include)

add_executable(zia
	Zia/src/main.cpp
    Zia/src/Zia.cpp)

target_link_libraries(zia ${CONAN_LIBS})
target_link_libraries(zia ${CMAKE_DL_LIBS})


add_library(SslConnectionModule SHARED
	modules/SslConnection/SslConnectionModule.cpp)

target_link_libraries(SslConnectionModule ${CONAN_LIBS})


add_library(HttpServerModule SHARED
	modules/HttpServer/HttpServer.cpp)

target_link_libraries(HttpServerModule ${CONAN_LIBS})


add_library(ParamsModule SHARED
	modules/Params/Params.cpp)

target_link_libraries(ParamsModule ${CONAN_LIBS})

add_library(ConfigParserModule SHARED
	modules/ConfigParser/ConfigParser.cpp)

target_link_libraries(ConfigParserModule ${CONAN_LIBS})

add_library(PhpCgiModule SHARED
	modules/PhpCgi/PhpCgi.cpp)

target_link_libraries(PhpCgiModule ${CONAN_LIBS})
