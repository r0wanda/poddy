cmake_minimum_required(VERSION 3.23)

add_library(LastFm)

target_sources(LastFm
	PRIVATE
		lastfm.cpp
	PUBLIC
		TYPE HEADERS
		FILES
			lastfm.hpp
)

target_link_libraries(LastFm PRIVATE GpodXX)
