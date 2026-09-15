cmake_minimum_required(VERSION 3.23)

project(Poddy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(poddy)

add_compile_options(-Wall -Wextra)

if (UNIX)
set(IS_LINUX 1)
configure_file(src/config.h.in src/config.h)
endif()

target_sources(poddy
    PRIVATE
        src/main.cpp
		src/poddy.cpp
		src/poddy.hpp
)

add_subdirectory(external/ftxui)
add_subdirectory(external/curlpp)
add_subdirectory(src/tui)
add_subdirectory(src/gpod)

target_link_libraries(poddy PRIVATE PoddyTui GpodXX)
