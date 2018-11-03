cmake_minimum_required(VERSION 3.5.1)
set(NAME snek)
project(${NAME})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../../cmake/Modules/")

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

set(SOURCES
	app.cpp
	audio.cpp
	game.cpp
	main.cpp
	random.cpp
	view.cpp
	)

add_executable(${NAME} ${SOURCES})
target_include_directories(${NAME} PRIVATE ${SDL2_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIRS})
target_link_libraries(${NAME} ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARIES})
