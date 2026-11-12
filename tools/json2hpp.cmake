cmake_minimum_required(VERSION 3.23)

project(json2hpp LANGUAGES CXX)

add_executable(json2hpp)

target_sources(json2hpp 
	PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/json2hpp.cpp
)

# not sure if this is the right way to do this in cmake

function(gen_json TGT JSON_NAME JSON_PATH)
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${JSON_NAME}.cpp
	COMMAND json2hpp ${JSON_NAME} ${CMAKE_CURRENT_LIST_DIR}/${JSON_PATH}
	DEPENDS json2hpp
)
target_sources(${TGT} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${JSON_NAME}.cpp)
endfunction()
