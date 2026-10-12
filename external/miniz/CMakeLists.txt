cmake_minimum_required(VERSION 3.23)

project(miniz VERSION 3.1.2 LANGUAGES C)

add_library(miniz)

target_sources(miniz
	PRIVATE
		miniz.c
	PUBLIC
		FILE_SET minizheaders
		TYPE HEADERS
		FILES
			miniz.h
)
