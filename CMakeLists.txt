cmake_minimum_required(VERSION 3.10)

project(test VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

configure_file(config.h.in config.h)
add_compile_definitions(MESSAGE_LEN=256 MESSAGE_SOURCE=10)

add_subdirectory(src)

add_executable(test test.c)
target_link_libraries(test PUBLIC libUSVMessage)

include_directories(
	"${PROJECT_SOURCE_DIR}/src"
	)
