# libpicam - RPi camera library

cmake_minimum_required (VERSION 2.8)
project (pigpio)

# Includes
include_directories (include)

# Set the output folders
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Set compiler flags
if (CMAKE_COMPILER_IS_GNUCC)
	set (GCC_COMPILE_FLAGS "-std=c99 -Wall -pedantic -g")
	
	execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
					OUTPUT_VARIABLE GCC_VERSION)
	if (GCC_VERSION VERSION_GREATER 4.9 OR GCC_VERSION VERSION_EQUAL 4.9)
		set (GCC_COMPILE_FLAGS "${GCC_COMPILE_FLAGS} -fdiagnostics-color=auto")
	endif()
	set (CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${GCC_COMPILE_FLAGS}")
endif()

# Add the source files
add_subdirectory(src)