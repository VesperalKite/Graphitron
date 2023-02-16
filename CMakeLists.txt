cmake_minimum_required(VERSION 3.5)
project(facet)

set(CMAKE_CXX_COMPILER "/usr/bin/c++")#remove to use default cxx compiler
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/facet_compiler")

# build the compiler
include_directories(./include/)
add_subdirectory(src)

# header files
file(GLOB_RECURSE HEADER_FILES include/*.h)

file(GLOB_RECURSE SOURCE_FILES include/*.h include/facet/midend/*.h src/*.cpp src/frontend/*.cpp src/midend/*.cpp src/backend/*.cpp src/utils/*.cpp src/utils/*.h include/utils/*.h)
add_executable(facetc ${SOURCE_FILES})

# build a front end library used for unit testing
file(GLOB_RECURSE LIB_SOURCE_FILES include/*.h src/frontend/*.cpp src/midend/*.cpp src/backend/*.cpp src/utils/*.cpp src/main.cpp)

add_library(facetlib ${HEADER_FILES} ${LIB_SOURCE_FILES})

set(FACETC_PY
	"${CMAKE_BINARY_DIR}/bin/facet_compiler/facetc.py"
)
add_custom_command(OUTPUT ${FACETC_PY}
	COMMAND sed -e s?\$\{CXX_COMPILER\}?${CMAKE_CXX_COMPILER}?g -e s?\$\{FACET_SOURCE_DIRECTORY\}?${CMAKE_SOURCE_DIR}?g -e s?\$\{FACET_BUILD_DIRECTORY\}?${CMAKE_BINARY_DIR}?g ${CMAKE_SOURCE_DIR}/src/facetc.py > ${FACETC_PY}
	#COMMAND ${CMAKE_COMMAND} -E copy ${FACETC_PY} ${CMAKE_BINARY_DIR}/bin/facetc.py
	DEPENDS ${CMAKE_SOURCE_DIR}/src/facetc.py
	VERBATIM
)
add_custom_target(copy_graphitc_py ALL DEPENDS ${FACETC_PY})
configure_file(src/main.cpp ${CMAKE_BINARY_DIR}/bin/facet_compiler/main.cpp COPYONLY)

add_custom_target(facet_apps_gen ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/apps
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/apps ${CMAKE_BINARY_DIR}/bin/apps
	COMMENT "Generating apps dir..."
)
add_custom_target(facet_output_gen ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/output
	COMMENT "Generating output dir..."
)
add_custom_target(facet_graphs_gen ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/graphs
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/graphs ${CMAKE_BINARY_DIR}/bin/graphs
	COMMENT "Generating graphs dir..."
)
add_custom_target(makefile_gen ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/mks ${CMAKE_BINARY_DIR}/bin/
    COMMENT "Generating makefile files..."
)