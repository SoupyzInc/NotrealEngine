cmake_minimum_required(VERSION 3.22)
project(NotrealEngine)

set(CMAKE_CXX_STANDARD 14)

# Dependencies
# ============
include_directories(lib/STB) # STB header files
include_directories(lib/GLAD/include) # Include GLAD header files
include_directories(lib/GLFW/glfw-3.3.7/include) # GLFW header files
add_subdirectory(lib/GLFW/glfw-3.3.7) # GLFW CMakeList.txt
include_directories(lib/GLM) # GLM header files

# Build Executable
# ================
set(testMode OFF)

if (NOT testMode)
    message(STATUS "Value testMode is set to OFF. Using src/main.cpp as entry.")
    add_executable(NotrealEngine
        src/main.cpp
        lib/GLAD/glad.c
        shaders/shader.h
        lib/STB/stb_image.h
        lib/STB/stb_image.cpp
        lib/camera/camera.h
    )
else()
    message(STATUS "Value testMode is set to ON. Using src/test.cpp as entry.")
    add_executable(NotrealEngine
        src/test.cpp
        lib/GLAD/glad.c
        shaders/shader.h
        lib/STB/stb_image.h
        lib/STB/stb_image.cpp
        lib/camera/camera.h)
endif()

# Link Libraries
# ==============
target_link_libraries(NotrealEngine -lOpenGL32 -lglu32) # OpenGL
target_link_libraries(NotrealEngine glfw) # GLFW
