cmake_minimum_required(VERSION 3.23)

add_library(PoddyTui)

target_sources(PoddyTui
	PRIVATE
		tui.cpp
	PUBLIC
		FILE_SET headers
		TYPE HEADERS
		FILES
			tui.hpp
)

#target_include_directories(PoddyTui PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(PoddyTui PUBLIC ftxui::screen ftxui::dom ftxui::component)
target_link_libraries(PoddyTui PUBLIC ftxui::screen ftxui::dom ftxui::component)
