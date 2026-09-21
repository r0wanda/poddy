cmake_minimum_required(VERSION 3.23)

add_library(PodMod)

target_sources(PodMod
	PRIVATE
		podmod.cpp
	PUBLIC
		TYPE HEADERS
		FILES
			podmod.hpp
)

