cmake_minimum_required(VERSION 3.23)

add_library(PoddyTui)

message("finding ftxui")
include(FetchContent)
FetchContent_Declare(ftxui
	GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
	GIT_TAG v7.0.3
)
FetchContent_MakeAvailable(ftxui)

target_sources(PoddyTui
	PRIVATE
		tui.cpp
	PUBLIC
		FILE_SET headers
		TYPE HEADERS
		FILES
			tui.hpp
)

target_include_directories(PoddyTui PUBLIC ftxui::screen ftxui::dom ftxui::component)
target_link_libraries(PoddyTui PUBLIC ftxui::screen ftxui::dom ftxui::component)
