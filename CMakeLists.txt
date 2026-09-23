cmake_minimum_required(VERSION 3.23)

project(Poddy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(poddy)

add_compile_options(-Wall -Wextra)

if (APPLE)
add_compile_definitions("IS_MACOS")
elseif (UNIX)
add_compile_definitions("IS_LINUX")
elseif (WIN32)
add_compile_definitions("IS_WINDOWS")
endif()

target_sources(poddy
	PUBLIC
		src/config.hpp
    PRIVATE
		src/config.cpp
		src/poddy.cpp
		src/poddy.hpp
		src/main.cpp
)

add_subdirectory(external/ftxui)
add_subdirectory(external/curlpp)
add_subdirectory(external/nlohmann)
add_subdirectory(src/podmod)
add_subdirectory(src/settings)
add_subdirectory(src/tui)
add_subdirectory(src/gpod)
add_subdirectory(src/finddev)

target_link_libraries(poddy PRIVATE PodMod PoddySettings PoddyTui GpodXX FindDev)
