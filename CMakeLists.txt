cmake_minimum_required(VERSION 3.0)

project(dice)

add_executable(
	${PROJECT_NAME}

	Main.cpp
	glad.c
	)

target_link_libraries(${PROJECT_NAME} glfw3)
