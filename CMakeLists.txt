cmake_minimum_required(VERSION 3.0)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	set(CMAKE_CXX_FLAGS "-DWINDOWS -O2 -static-libstdc++ -static-libgcc -mwindows -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,-Bdynamic,--no-whole-archive")	
else()
	set(CMAKE_CXX_FLAGS "-Wl,-rpath=${ORIGIN} -O2 -flto -static-libstdc++ -static-libgcc")		
endif()

project(dice)

find_package(OpenGL REQUIRED)

add_executable(
	${PROJECT_NAME}

	Main.cpp
	glad.c
	
	Shader.cpp
	Texture.cpp

	Map.cpp
	
	Player.cpp
	
	Enemy.cpp
	Bomb.cpp
	)

target_link_libraries(${PROJECT_NAME} SOIL2)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} glfw3)
