cmake_minimum_required(VERSION 3.23)

add_library(PodMod)

target_sources(PodMod
	PRIVATE
 		podmod.cpp
	PUBLIC
		podmod.hpp
)

target_include_directories(PodMod PRIVATE ftxui::screen ftxui::dom ftxui::component)
target_link_libraries(PodMod PRIVATE ftxui::screen ftxui::dom ftxui::component)
