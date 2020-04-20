cmake_minimum_required(VERSION 3.10)

project(test VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

configure_file(config.h.in config.h)

add_subdirectory(src)

add_executable(test test.c)
target_link_libraries(test PUBLIC libUSVMessage)

target_include_directories(test PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/src"
	)
