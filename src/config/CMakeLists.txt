cmake_minimum_required(VERSION 3.23)

add_library(PoddyConfig)

target_sources(PoddyConfig
	PRIVATE
		config.cpp
	PUBLIC
		FILE_SET configheaders
		TYPE HEADERS
		FILES
			config.hpp
)


target_link_libraries(PoddyConfig)
