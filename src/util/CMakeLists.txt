cmake_minimum_required(VERSION 3.23)

add_library(PodUtil)

target_sources(PodUtil
	PRIVATE
 		util.cpp
	PUBLIC
		FILE_SET utilheaders
		TYPE HEADERS
		FILES
			util.hpp
)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
target_include_directories(PodUtil PRIVATE ${GLIB_INCLUDE_DIRS})
#target_link_libraries(PodUtil PRIVATE ${GLIB_LIBRARIES})
