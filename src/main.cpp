#include <climits>
#include <direct.h>
#include <iomanip>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../lib/GLM/glm.hpp"
#include "../lib/GLM/gtc/matrix_transform.hpp"
#include "../lib/GLM/gtc/type_ptr.hpp"

#include "../shaders/shader.h"
#include "stb_image.h"

using namespace std;

const unsigned int WIDTH = 1600 * 1.5;
const unsigned int HEIGHT = 900 * 1.5;

float mixValue = 0.2f;

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  static_cast<float>(WIDTH) / 2.0;
float lastY =  static_cast<float>(HEIGHT) / 2.0;
float fov   =  45.0f;

double deltaTime = 0.0f; // Time between current frame and last frame in seconds.
double lastFrame = 0.0f; // Time of last frame.

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

glm::vec3 lightPos    = glm::vec3(1.0f, 2.0f, 0.0f);

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
	auto cameraSpeed = static_cast<float>(2.5f * deltaTime); // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraFront;
    }
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraFront;
    }
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
	
	// Debug
	// -----
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (mixValue < 1.0f) {
			mixValue += static_cast<float>(2.5f * deltaTime);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (mixValue > 0.0f) {
			mixValue -= static_cast<float>(2.5f * deltaTime);
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

int main() {
#pragma region Program Setup
	glfwInit(); // First initialize GLFW.
	
	// Using GLFW 3.3.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
	// Using core profile (not using backwards compatible features).
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	// Create window
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Notreal Engine | Loading debug information...", nullptr, nullptr);
	
	// Error handling for failed window creation.
	if (window == nullptr) {
		std::cout << "ERROR::GLFW::WINDOW_CREATION_FAILED" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); // Make context of window the main context on the current thread.
	glfwSwapInterval( 0 ); // Disable vsync.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide cursor and capture position.
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // GLAD manages function pointers for OpenGL. Therefore, we want to initialize it before calling
	// any OpenGL functions.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "ERROR::GLAD::INITIALIZATION_FAILED" << std::endl;
		return -1;
	}
	
	glViewport(0, 0, WIDTH, HEIGHT); // Set viewport position (bottom left) and size.
	
	// Tell GLFW that we want to call this function when window size changes.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);
#pragma endregion

#pragma region Shaders
    // Shaders
    // -------
	// Get current working directory.
	char buff[PATH_MAX];
	_getcwd(buff, PATH_MAX);
	string current_working_dir(buff);
	current_working_dir = current_working_dir.substr(0, 43);
	
//	// char[] for vertex shader directory.
//	char VERTEX_SHADER_DIR[current_working_dir.size() + strlen("shaders\\vertexShader.glsl")];
//	strcpy(VERTEX_SHADER_DIR, (current_working_dir + "shaders\\vertexShader.glsl").c_str());
//
//	// char[] for fragment shader directory.
//	char FRAGMENT_SHADER_DIR[current_working_dir.size() + strlen("shaders\\fragmentShader.glsl")];
//	strcpy(FRAGMENT_SHADER_DIR, (current_working_dir + "shaders\\fragmentShader.glsl").c_str());
//
//	// Create shader.
//	Shader ourShader(VERTEX_SHADER_DIR, FRAGMENT_SHADER_DIR);

    // Lighting Shaders
    // ----------------
    // char[] for lighting vertex shader directory.
    char LIGHTING_VERTEX_SHADER_DIR[current_working_dir.size() + strlen("shaders\\lighting.vs")];
    strcpy(LIGHTING_VERTEX_SHADER_DIR, (current_working_dir + "shaders\\lighting.vs").c_str());

    // char[] for lighting fragment shader directory.
    char LIGHTING_FRAGMENT_SHADER_DIR[current_working_dir.size() + strlen("shaders\\lighting.fs")];
    strcpy(LIGHTING_FRAGMENT_SHADER_DIR, (current_working_dir + "shaders\\lighting.fs").c_str());

    Shader lightingShader(LIGHTING_VERTEX_SHADER_DIR, LIGHTING_FRAGMENT_SHADER_DIR);

    // Lighting Shaders
    // ----------------
    // char[] for lighting vertex shader directory.
    char LIGHT_CUBE_VERTEX_SHADER_DIR[current_working_dir.size() + strlen("shaders\\lightCube.vs")];
    strcpy(LIGHT_CUBE_VERTEX_SHADER_DIR, (current_working_dir + "shaders\\lightCube.vs").c_str());

    // char[] for lighting fragment shader directory.
    char LIGHT_CUBE_FRAGMENT_SHADER_DIR[current_working_dir.size() + strlen("shaders\\lightCube.fs")];
    strcpy(LIGHT_CUBE_FRAGMENT_SHADER_DIR, (current_working_dir + "shaders\\lightCube.fs").c_str());

    Shader lightCubeShader(LIGHT_CUBE_VERTEX_SHADER_DIR, LIGHT_CUBE_FRAGMENT_SHADER_DIR);
#pragma endregion

#pragma region User Defined Shapes
	float vertices[] = {
		// Positions        	// Normals
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f, 	0.0f, 0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f, 	0.0f, 0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f, 	0.0f, 0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f, 	0.0f, 0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f, 	0.0f, 0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, 	0.0f, 0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, 	-1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 	-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 	-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 	-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 	-1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 	-1.0f, 0.0f, 0.0f,

		 0.5f,  0.5f,  0.5f, 	1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 	1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 	1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 	1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 	1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 	1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 	0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 	0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 	0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 	0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 	0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 	0.0f, -1.0f, 0.0f,

		-0.5f,  0.5f, -0.5f, 	0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 	0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 	0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 	0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 	0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 	0.0f, 1.0f, 0.0f
	};
	
	glm::vec3 cubePositions[] = {
		glm::vec3( -5.0f,  0.0f, -5.0f)
//		glm::vec3(-1.7f,  3.0f,  -7.5f),
//		glm::vec3( 1.3f, -2.0f,  -2.5f),
//		glm::vec3( 1.5f,  2.0f,  -2.5f),
//		glm::vec3( 1.5f,  0.2f,  -1.5f),
//		glm::vec3(-1.3f,  1.0f,  -1.5f),
//		glm::vec3( 2.0f,  5.0f, -15.0f),
//		glm::vec3(-1.5f, -2.2f,  -2.5f),
//		glm::vec3(-3.8f, -2.0f, -12.3f),
//		glm::vec3( 2.4f, -0.4f,  -3.5f)
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)nullptr);
    glEnableVertexAttribArray(0);

    // Set normal attribute pointers.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Generate Light VAO
    // ------------------
    unsigned int lightVAO; // VAO for light cube.
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    // Only VBO needs to be bound, the container's VBO data already has the needed data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Set vertex attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)nullptr);
    glEnableVertexAttribArray(0);

    // Generate EBO
	// ------------
//	unsigned int EBO;
//	glGenBuffers(1, &EBO);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	// Textures
	// --------
	// Set vertex texture attribute pointers.
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//	glEnableVertexAttribArray(2);
//
//	glEnable(GL_BLEND); // Enable blending.
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Allow transparency to work with PNGs.
#pragma endregion

#pragma region User Defined Textures
//	unsigned int texture1, texture2;
//	int width, height, nrChannels;
//
//	// Texture 1
//	// ---------
//	glGenTextures(1, &texture1); // Set ID to texture.
//	glBindTexture(GL_TEXTURE_2D, texture1);
//
//	// Set texture wrapping and filtering.
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use more pixelated filtering to scale down.
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Use more blurry filtering to scale up.
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	// Load image data.
//	char TEXTURE1_DIR[current_working_dir.size() + strlen(R"(textures\container.jpg)")];
//	strcpy(TEXTURE1_DIR, (current_working_dir + R"(textures\container.jpg)").c_str());
//	unsigned char *data = stbi_load(TEXTURE1_DIR, &width, &height, &nrChannels, 0);
//
//	if (data) {
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	} else {
//		cout << "ERROR::TEXTURE::DATA_NOT_SUCCESSFULLY_LOADED\nAttempted to load texture from " << TEXTURE1_DIR << endl;
//	}
//
//	stbi_image_free(data); // Free memory.
//
//	// Texture 2
//	// ---------
//	stbi_set_flip_vertically_on_load(true); // From here on, tell stb_image.h to flip loaded textures on y-axis (only flip texture 2).
//
//	glGenTextures(1, &texture2); // Set ID to texture.
//	glBindTexture(GL_TEXTURE_2D, texture2);
//
//	// Set texture wrapping and filtering.
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use more pixelated filtering to scale down.
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Use more blurry filtering to scale up.
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	// Load image data.
//	char TEXTURE2_DIR[current_working_dir.size() + strlen(R"(textures\awesomeface.png)")];
//	strcpy(TEXTURE2_DIR, (current_working_dir + R"(textures\awesomeface.png)").c_str());
//	data = stbi_load(TEXTURE2_DIR, &width, &height, &nrChannels, 0);
//	if (data) {
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	} else {
//		cout << "ERROR::TEXTURE::DATA_NOT_SUCCESSFULLY_LOADED\nAttempted to load texture from " << TEXTURE2_DIR << endl;
//	}
//
//	stbi_image_free(data); // Free memory.
#pragma endregion

//	ourShader.use(); // Activate our shader before use.
//
//	// Tell OpenGL which texture unit each shader sampler belongs to.
//	ourShader.setInt("texture1", 0); // With shader class
//	ourShader.setInt("texture2", 1);

	double currentFrame; // Time of current frame in seconds.
	unsigned int timePrev = 0; // Time of last frame rounded to two decimals.
	
	unsigned int averageFps; // The average FPS over last 10 seconds.
	unsigned int frames = 0; // How many frames have passed in one second.
	unsigned int counter = 1; // Index of FPSs.
	
	double FPSs[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Stores FPSs over last 10 seconds to calculate SMA10.

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

			averageFps = static_cast<unsigned int>((FPSs[0] + FPSs[1] + FPSs[2] + FPSs[3] + FPSs[4] + FPSs[5] + FPSs[6] + FPSs[7] + FPSs[8] + FPSs[9]) / 10);
			
			string newTitle = "Notreal Engine | " + to_string(frames) + " FPS/" + to_string(averageFps) + " AFPS | " + to_string(deltaTime * 1000) + " MSPF | " +to_string(timeNow / 100) + "S";
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

        // Bind Textures
        // -------------
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, texture1);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, texture2);
//
//		ourShader.setFloat("mixValue", mixValue); // Mix textures

        // Shading
        // -------
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor",  0.5f, 0.5f, 0.5f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", cameraPos);

        // The projection matrix.
        glm::mat4 projection = glm::perspective(glm::radians(fov), static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.01f, 100.0f);
        // The view matrix.
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // The model matrix holds translations, scaling, and/or rotations that transform all object's vertices to the global world space.
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(5.0f, -0.025f, 5.0f));
//        model = glm::translate(model, glm::vec3(-0.5f, 0.0f, -2.0f));
//        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        lightingShader.setMat4("model", model);

        // Draw cube object.
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw light cube.
        lightCubeShader.use();

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube

        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window); // Swaps color buffer.
		glfwPollEvents(); // Checks if any events are triggered.
	}
	
	// Relieve buffers.
	glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
//	glDeleteBuffers(1, &EBO);

	// Terminate GLFW and relieve all allocated GLFW resources.
	glfwTerminate();
	return 0;
}