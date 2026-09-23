cmake_minimum_required(VERSION 3.23)

add_library(FindDev)

target_sources(FindDev
	PRIVATE
		finddev.cpp
	PUBLIC
		finddev.hpp
)

if (UNIX)
find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBUDISKS REQUIRED udisks2)

target_include_directories(FindDev PUBLIC ${LIBUDISKS_INCLUDE_DIRS})
target_compile_options(FindDev PRIVATE ${LIBUDISKS_CFLAGS_OTHER})
target_link_libraries(FindDev PRIVATE ${LIBUDISKS_LIBRARIES})
endif()

target_include_directories(FindDev PRIVATE ftxui::screen ftxui::dom ftxui::component)
target_link_libraries(FindDev PRIVATE ftxui::screen ftxui::dom ftxui::component)
