cmake_minimum_required(VERSION 3.23)

add_library(PoddyMain)

target_sources(PoddyMain
	PRIVATE
		poddy.cpp
	PUBLIC
		FILE_SET poddyheaders
		TYPE HEADERS
		FILES
			poddy.hpp
)


target_link_libraries(PoddyMain PRIVATE PoddyConfig ftxui::screen ftxui::dom ftxui::component PoddyTui GpodXX FindDev` PodMod PodSettings)
