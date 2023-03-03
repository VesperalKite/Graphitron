cmake_minimum_required(VERSION 3.5)
project(graphitron)

set(CMAKE_CXX_COMPILER "/usr/bin/c++")#remove to use default cxx compiler
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/graphitron_compiler")

# build the compiler
include_directories(./include/)
add_subdirectory(src)

# header files
file(GLOB_RECURSE HEADER_FILES include/*.h)

file(GLOB_RECURSE SOURCE_FILES include/*.h include/graphitron/midend/*.h src/*.cpp src/frontend/*.cpp src/midend/*.cpp src/backend/*.cpp src/utils/*.cpp src/utils/*.h include/utils/*.h)
add_executable(graphitronc ${SOURCE_FILES})

# build a front end library used for unit testing
file(GLOB_RECURSE LIB_SOURCE_FILES include/*.h src/frontend/*.cpp src/midend/*.cpp src/backend/*.cpp src/utils/*.cpp src/main.cpp)

add_library(graphitronlib ${HEADER_FILES} ${LIB_SOURCE_FILES})

set(GRAPHITRONC_PY
	"${CMAKE_BINARY_DIR}/bin/graphitron_compiler/graphitronc.py"
)
add_custom_command(OUTPUT ${GRAPHITRONC_PY}
	COMMAND sed -e s?\$\{CXX_COMPILER\}?${CMAKE_CXX_COMPILER}?g -e s?\$\{GRAPHITRON_SOURCE_DIRECTORY\}?${CMAKE_SOURCE_DIR}?g -e s?\$\{GRAPHITRON_BUILD_DIRECTORY\}?${CMAKE_BINARY_DIR}?g ${CMAKE_SOURCE_DIR}/src/graphitronc.py > ${GRAPHITRONC_PY}
	#COMMAND ${CMAKE_COMMAND} -E copy ${GRAPHITRON_PY} ${CMAKE_BINARY_DIR}/bin/graphitronc.py
	DEPENDS ${CMAKE_SOURCE_DIR}/src/graphitronc.py
	VERBATIM
)
add_custom_target(copy_graphitc_py ALL DEPENDS ${GRAPHITRONC_PY})
configure_file(src/main.cpp ${CMAKE_BINARY_DIR}/bin/graphitron_compiler/main.cpp COPYONLY)

add_custom_target(graphitron_apps_gen ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/apps
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/apps ${CMAKE_BINARY_DIR}/bin/apps
	COMMENT "Generating apps dir..."
)
add_custom_target(graphitron_output_gen ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/fpga_accelerator
	COMMENT "Generating output dir..."
)
add_custom_target(graphitron_graphs_gen ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/graphs
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/graphs ${CMAKE_BINARY_DIR}/bin/graphs
	COMMENT "Generating graphs dir..."
)
add_custom_target(makefile_gen ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/mks ${CMAKE_BINARY_DIR}/bin/
    COMMENT "Generating makefile files..."
)