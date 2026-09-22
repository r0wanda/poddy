cmake_minimum_required(VERSION 3.23)

add_library(GpodXX)

target_sources(GpodXX
	PRIVATE
		gpod.cpp
	PUBLIC
		TYPE HEADERS
		FILES
			gpod.hpp
)

#target_include_directories(GpodXX PRIVATE ftxui::screen ftxui::dom ftxui::component)
#target_link_libraries(GpodXX PRIVATE ftxui::screen ftxui::dom ftxui::component)

if (UNIX)
find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBGPOD REQUIRED libgpod-1.0)

target_include_directories(GpodXX PUBLIC ${LIBGPOD_INCLUDE_DIRS})
target_compile_options(GpodXX PUBLIC ${LIBGPOD_CFLAGS_OTHER})
target_link_libraries(GpodXX PRIVATE ${LIBGPOD_LIBRARIES})
endif()

#target_include_directories(GpodXX PUBLIC ${CMAKE_CURRENT_LIST_DIR})

