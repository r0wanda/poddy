cmake_minimum_required(VERSION 3.23)

add_library(PodMod)

target_sources(PodMod
	PRIVATE
 		podmod.cpp
		${PROJECT_SOURCE_DIR}/external/miniz/miniz.c
		${PROJECT_SOURCE_DIR}/external/miniz/miniz.h
	PUBLIC
		podmod.hpp
)

target_include_directories(PodMod PRIVATE ftxui::screen ftxui::dom ftxui::component)
target_link_libraries(PodMod PRIVATE ftxui::screen ftxui::dom ftxui::component)
