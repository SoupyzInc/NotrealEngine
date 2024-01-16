#include <direct.h>
#include <climits>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../lib/GLM/glm.hpp"
#include "../lib/GLM/gtc/matrix_transform.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../shaders/shader.h"
#include "stb_image.h"

#include "actor.h"

#include "../lib/camera/camera.h"

using namespace std;

string globalDir;

const unsigned int WIDTH = 1600 * 1.5;
const unsigned int HEIGHT = 900 * 1.5;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

double deltaTime = 0.0f; // Time between current frame and last frame in seconds.
double lastFrame = 0.0f; // Time of last frame.

const glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

// Adjust viewport to resize with window resizes.
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Handles key inputs during program execution.
void processInput(GLFWwindow *window) {
    // If ESC, then close window.
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Camera Function
    // ---------------
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    // Debug
    // -----
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Left click
    bool click = false;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);

        if (!io.WantCaptureMouse) { click = true; }
    }

    if (click) { camera.left_click = true; } else { camera.left_click = false; }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        click = false;
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE);
    }

    // Right click
    click = false;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS);

        if (!io.WantCaptureMouse) { click = true; }

    }

    if (click) { camera.right_click = true; } else { camera.right_click = false; }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
        click = false;
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE);
    }
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    auto xpos = static_cast<float>(xposIn);
    auto ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// Keeps track of scroll displacement.
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {
        camera.ProcessMouseScroll(static_cast<float>(yoffset), deltaTime);
    }
}

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

unsigned int loadTextures(const char *fileName) {
    unsigned int textureId;
    int width, height, nrChannels;

    glGenTextures(1, &textureId); // Set ID to texture.

    // Char[] for vertex shader directory.
    char TEXTURE1_DIR[globalDir.size() + strlen(R"(Textures\)") + strlen(fileName)];
    strcpy(TEXTURE1_DIR, (globalDir + R"(Textures\)" + fileName).c_str());
    unsigned char *data = stbi_load(TEXTURE1_DIR, &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format;
        if (nrChannels == 1) {
            format = GL_RED;
        } else if (nrChannels == 3) {
            format = GL_RGB;
        } else if (nrChannels == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping and filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_NEAREST); // Use more pixelated filtering to scale down.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Use more blurry filtering to scale up.

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        cout << "Failed to load texture." << endl;
    }

    stbi_image_free(data); // Free memory.
    return textureId;
}

void updateActors(actor actors[], int n) {
    for (int i = 0; i < n; ++i) {
        if (i == 0)
            actors[i].Position.x += 0.001;

        if (i == 1)
            actors[i].Position.y += 0.001;

        if (i == 2)
            actors[i].Position.z += 0.001;

    }
}

int main() {
#pragma region Program Setup
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
    // GLFW
    glfwInit(); // First initialize GLFW.

    // Using GLFW 3.3.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Using core profile (not using backwards compatible features).
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window.
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Notreal Engine | Loading debug information...", nullptr,
                                          nullptr);

    // Error handling for failed window creation.
    if (window == nullptr) {
        std::cout << "ERROR::GLFW::WINDOW_CREATION_FAILED" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Make context of window the main context on the current thread.
    glfwSwapInterval(0); // Disable vsync.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Hide cursor and capture position.
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // GLAD manages function pointers for OpenGL. Therefore, we want to initialize it before calling any OpenGL functions.
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "ERROR::GLAD::INITIALIZATION_FAILED" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT); // Set viewport position (bottom left) and size.

    // Tell GLFW that we want to call this function when window size changes.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Enable depth testing.
    glEnable(GL_DEPTH_TEST);

    // IMGUI
    // -----
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
#pragma endregion

#pragma region Shaders
    // Shaders
    // -------
    // Get current working directory.
    char buff[PATH_MAX];
    _getcwd(buff, PATH_MAX);
    string current_working_dir(buff);
    globalDir = current_working_dir.substr(0, 43);

    // Lighting Shaders
    // ----------------
    // char[] for lighting vertex shader directory.
    char LIGHTING_VERTEX_SHADER_DIR[globalDir.size() + strlen("shaders\\lighting.vs")];
    strcpy(LIGHTING_VERTEX_SHADER_DIR, (globalDir + "shaders\\lighting.vs").c_str());

    // char[] for lighting fragment shader directory.
    char LIGHTING_FRAGMENT_SHADER_DIR[globalDir.size() + strlen("shaders\\lighting.fs")];
    strcpy(LIGHTING_FRAGMENT_SHADER_DIR, (globalDir + "shaders\\lighting.fs").c_str());

    Shader lightingShader(LIGHTING_VERTEX_SHADER_DIR, LIGHTING_FRAGMENT_SHADER_DIR);

    // Lighting Shaders
    // ----------------
    // char[] for light cube vertex shader directory.
    char LIGHT_CUBE_VERTEX_SHADER_DIR[globalDir.size() + strlen("shaders\\lightCube.vs")];
    strcpy(LIGHT_CUBE_VERTEX_SHADER_DIR, (globalDir + "shaders\\lightCube.vs").c_str());

    // char[] for light cube fragment shader directory.
    char LIGHT_CUBE_FRAGMENT_SHADER_DIR[globalDir.size() + strlen("shaders\\lightCube.fs")];
    strcpy(LIGHT_CUBE_FRAGMENT_SHADER_DIR, (globalDir + "shaders\\lightCube.fs").c_str());

    Shader lightCubeShader(LIGHT_CUBE_VERTEX_SHADER_DIR, LIGHT_CUBE_FRAGMENT_SHADER_DIR);
#pragma endregion

#pragma region User Defined Shapes
    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)
    };
#pragma endregion

#pragma region VBO, VBA, EBO, Textures
    // Create VBO
    // ----------
    // Vertex buffer object (VBO) - stores a large number of vertices in the GPU's memory.
    unsigned int VBO;
    unsigned int VAO; // VAO for object cube.
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind aforementioned buffer to array buffer type. From here on, all buffer calls on the
    // GL_ARRAY_BUFFER target will be used to configure the bound buffer, VBO.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copy the triangles vertices into the buffer's memory. glBufferData() is specifically
    // targeted to copy user-defined data into the buffer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO); // Bind VAO

    // Set vertex position attribute pointers.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);

    // Set normal attribute pointers.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Set texture attribute pointers.
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Generate Light VAO
    // ------------------
    unsigned int lightVAO; // VAO for light cube.
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    // Only VBO needs to be bound, the container's VBO data already has the needed data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Set vertex attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);
#pragma endregion

    // FPS Debug Variables
    // -------------------
    double currentFrame; // Time of current frame in seconds.
    unsigned int timePrev = 0; // Time of last frame rounded to two decimals.

    unsigned int averageFps; // The average FPS over last 10 seconds.
    unsigned int frames = 0; // How many frames have passed in one second.
    unsigned int counter = 1; // Index of FPSs.

    double FPSs[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Stores FPSs over last 10 seconds to calculate SMA10.

    // Render Variables
    // ----------------
    unsigned int diffuseMap = loadTextures("container2.png");
    unsigned int specularMap = loadTextures("container2_specular.png");
    unsigned int emissionMap = loadTextures("container2_emission.png");

    lightingShader.use();
    lightingShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    lightingShader.setInt("material.emission", 2);

    // Actors
    // ------
    actor a1(glm::vec3(0.0f,0.0f,0.0f));
    actor a2(glm::vec3(0.0f,0.0f,0.0f));
    actor a3(glm::vec3(0.0f,0.0f,0.0f));
    actor actors[3] = {a1, a2, a3};

    // Render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // FPS Debug Information
        // ---------------------
        frames++;
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        auto timeNow = static_cast<unsigned int>(currentFrame * 100); // Use 2 decimal places of accuracy.

        // Skipped second due to low TPS or window interruption.
        if (timeNow / 100 - timePrev / 100 > 1) {
            timePrev = timeNow; // Reset second check cycle.
        }

        // If approximately one second has passed.
        if (timeNow - timePrev == 100) {
            FPSs[counter - 1] = frames;

            if (counter % 10 == 0) {
                counter = 0;
            }

            averageFps = static_cast<unsigned int>(
                    (FPSs[0] + FPSs[1] + FPSs[2] + FPSs[3] + FPSs[4] + FPSs[5] + FPSs[6] + FPSs[7] + FPSs[8] + FPSs[9])
                    / 10);

            string newTitle = "Notreal Engine | " + to_string(frames) + " FPS/" + to_string(averageFps) + " AFPS | "
                              + to_string(deltaTime * 1000) + " MSPF | " + to_string(timeNow / 100) + "S";
            glfwSetWindowTitle(window, newTitle.c_str());

            frames = 0;
            timePrev = timeNow;
            counter++;
        }

        processInput(window); // Check for key inputs.

        // Rendering Commands
        // ------------------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Shading
        // -------
        lightingShader.use();
        lightingShader.setVec3("lightPos", lightPos);

        lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        lightingShader.setFloat("material.shininess", 32.0f);

        // Rendering
        // ---------
        // The projection matrix.
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                static_cast<float>(WIDTH) / static_cast<float>(HEIGHT),
                                                0.01f,
                                                100.0f);
        // The view matrix.
        glm::mat4 view = camera.GetViewMatrix();

        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // The model matrix holds translations, scaling, and/or rotations that transform all object's vertices to the global world space.
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // Bind textures.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        for (auto& a: actors) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, a.Position);
            lightingShader.setMat4("model", model);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

//        for (unsigned int i = 0; i < 10; i++) {
//            glm::mat4 model = glm::mat4(1.0f);
//            model = glm::translate(model, cubePositions[i]);
//            float angle = 20.0f * i;
//            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
//            lightingShader.setMat4("model", model);
//
//            glBindVertexArray(VAO);
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window); // Swaps color buffer.
        glfwPollEvents(); // Checks if any events are triggered.

        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
            // Update actors
            updateActors(actors, 3);
        }
    }

    // Relieve buffers.
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Terminate GLFW and relieve all allocated GLFW resources.
    glfwTerminate();
    return 0;
}