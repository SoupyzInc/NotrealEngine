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
include_directories(lib/imgui)
include_directories(lib/imgui/backends)

# IMGUI
# =====
set(IMGUI_SRC
        lib/imgui/misc/cpp/imgui_stdlib.cpp
        lib/imgui/misc/cpp/imgui_stdlib.h

        lib/imgui/imconfig.h
        lib/imgui/imgui.cpp
        lib/imgui/imgui.h
        lib/imgui/imgui_demo.cpp
        lib/imgui/imgui_draw.cpp
        lib/imgui/imgui_internal.h
        lib/imgui/imgui_tables.cpp
        lib/imgui/imgui_widgets.cpp
        lib/imgui/imstb_rectpack.h
        lib/imgui/imstb_textedit.h
        lib/imgui/imstb_truetype.h

        lib/imgui/backends/imgui_impl_glfw.h
        lib/imgui/backends/imgui_impl_glfw.cpp
        lib/imgui/backends/imgui_impl_opengl3.h
        lib/imgui/backends/imgui_impl_opengl3.cpp
)

# Build Executable
# ================
add_executable(NotrealEngine
    src/main.cpp
    lib/GLAD/glad.c
    shaders/shader.h
    lib/STB/stb_image.h
    lib/STB/stb_image.cpp
    lib/camera/camera.h
    ${IMGUI_SRC}
)

# Link Libraries
# ==============
target_link_libraries(NotrealEngine -lOpenGL32 -lglu32) # OpenGL
target_link_libraries(NotrealEngine glfw) # GLFW
