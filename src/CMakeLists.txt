# libpicam - source files

# Source and header files
set (SOURCE
	 rpio.c
	 cpuinfo.c
)
set (HEADERS
	 rpio-private.h
	 ../include/rpio.h
)

# Compile as a static library
add_library (rpio STATIC ${HEADERS} ${SOURCE})
add_executable (test_rpio test-rpio.c)
target_link_libraries(test_rpio LINK_PRIVATE rpio)