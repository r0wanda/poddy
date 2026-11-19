cmake_minimum_required(VERSION 3.23)

project(Poddy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)

add_executable(poddy)

add_compile_options(-Wall -Wextra -fdiagnostics-color=always)

set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "")

if (APPLE)
add_compile_definitions("IS_MACOS")
elseif (UNIX)
add_compile_definitions("IS_LINUX")
elseif (WIN32)
add_compile_definitions("IS_WINDOWS")
endif()

target_sources(poddy
    PRIVATE
		src/main.cpp
)

include(tools/json2hpp.cmake)
add_subdirectory(external/ftxui)
add_subdirectory(external/curlpp)
add_subdirectory(external/nlohmann)
add_subdirectory(external/json-schema-validator)
add_subdirectory(external/semver)
add_subdirectory(src/util)
add_subdirectory(src/podmod)
add_subdirectory(src/settings)
add_subdirectory(src/tui)
add_subdirectory(src/gpod)
add_subdirectory(src/finddev)
add_subdirectory(src/poddy)

target_link_libraries(poddy PRIVATE PodUtil PodMod PoddyMain)
