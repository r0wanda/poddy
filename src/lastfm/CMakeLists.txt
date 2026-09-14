cmake_minimum_required(VERSION 3.23)

add_library(GpodXX)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGPOD REQUIRED libgpod-1.0)

target_sources(GpodXX
	PRIVATE
		gpod.cpp
		finddev.cpp
	PUBLIC
		FILE_SET HEADERS
		FILES
			gpod.hpp
			finddev.hpp
)



#target_include_directories(GpodXX PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(GpodXX PUBLIC ${LIBGPOD_INCLUDE_DIRS})
target_compile_options(GpodXX PUBLIC ${LIBGPOD_CFLAGS_OTHER})
target_link_libraries(GpodXX PRIVATE ${LIBGPOD_LIBRARIES})
