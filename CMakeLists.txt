cmake_minimum_required(VERSION 3.22)
project(NotrealEngine)

set(CMAKE_CXX_STANDARD 14)

include_directories(lib/STB) # STB header files
include_directories(${PROJECT_SOURCE_DIR}/lib/GLAD/include) # Include GLAD header files
include_directories(${PROJECT_SOURCE_DIR}/lib/GLFW/glfw-3.3.7/include) # GLFW header files
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/GLFW/glfw-3.3.7) # GLFW CMakeList.txt

add_executable(NotrealEngine main.cpp lib/GLAD/glad.c Shaders/shader.h lib/STB/stb_image.h lib/STB/stb_image.cpp)

#target_link_libraries(NotrealEngine -lOpenGL32 -lfreeGLUT) # GLUT
target_link_libraries(NotrealEngine -lOpenGL32 -lglu32) # OpenGL
target_link_libraries(NotrealEngine glfw) # GLFW
