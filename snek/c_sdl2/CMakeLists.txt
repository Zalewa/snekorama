cmake_minimum_required(VERSION 3.5.1)
set(NAME SnekC)
project(${NAME})

set(CMAKE_C_STANDARD 90)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

set(SOURCES
	audio.c
	game.c
	list.c
	main.c
	vec2.c
	view.c
	)

add_executable(${NAME} ${SOURCES})
target_include_directories(${NAME} PRIVATE ${SDL2_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIRS})
target_link_libraries(${NAME} ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARIES})
