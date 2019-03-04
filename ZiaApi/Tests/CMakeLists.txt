cmake_minimum_required(VERSION 3.5)

set(NAME zany-tests-bin)

# execute_process(
# 	COMMAND ../../compile.sh --get-build-directory
# 	OUTPUT_VARIABLE OUTPUT_PATH)

project(${NAME})

set(CMAKE_CXX_STANDARD 17)
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -W -Wall -Wextra -Wno-missing-field-initializers -Wno-unused-parameter")
endif ()


include_directories(../lib)
include_directories(.)

set(SRCS
	main.cpp
	Entity/tests.cpp
	ThreadPool/tests.cpp
	Pipeline/tests.cpp
	Event/tests.cpp
	Context/tests.cpp
	Orchestrator/tests.cpp
	Orchestrator/testsLoader.cpp
)

link_directories(.)
add_executable(${NAME} ${SRCS})
target_link_libraries(${NAME} ${CMAKE_DL_LIBS})
find_package(Threads)
target_link_libraries(${NAME} ${CMAKE_THREAD_LIBS_INIT})

project(SimpleModule)
add_library(SimpleModule SHARED Orchestrator/SimpleModule.cpp)

project(SimpleParserModule)
add_library(SimpleParserModule SHARED Orchestrator/SimpleParserModule.cpp)
