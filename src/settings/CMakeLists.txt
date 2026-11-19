cmake_minimum_required(VERSION 3.23)

add_library(PodSettings)

target_sources(PodSettings
	PRIVATE
 		settings.cpp
	PUBLIC
		FILE_SET settingsheaders
		TYPE HEADERS
		FILES
			settings.hpp
)

gen_json(PodSettings manifestSchema)

target_include_directories(PodSettings PRIVATE nlohmann_json_schema_validator::validator)
target_link_libraries(PodSettings PRIVATE PodMod ftxui::screen ftxui::dom ftxui::component PUBLIC nlohmann_json_schema_validator::validator semver::semver)
