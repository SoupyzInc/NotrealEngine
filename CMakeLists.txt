cmake_minimum_required(VERSION 3.22)
project(Testing)

set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR}/GLAD/include) # Include GLAD header files
include_directories(${PROJECT_SOURCE_DIR}/GLFW/glfw-3.3.7/include) # GLFW header files
add_subdirectory(${PROJECT_SOURCE_DIR}/GLFW/glfw-3.3.7) # GLFW CMakeList.txt

add_executable(Testing main.cpp glad.c Shaders/shader.h)

#target_link_libraries(Testing -lOpenGL32 -lfreeGLUT) # GLUT
target_link_libraries(Testing -lOpenGL32 -lglu32) # OpenGL
target_link_libraries(Testing glfw) # GLFW
