#include <direct.h>
#include <climits>
#include <iostream>
#include <cstring>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shaders/shader.h"
#include "stb_image.h"

using namespace std;

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
	
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
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
	GLFWwindow *window = glfwCreateWindow(800, 600, "Nonreal Engine", nullptr, nullptr);
	
	// Error handling for failed window creation.
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); // Make context of window the main context on the current thread.
	
	// GLAD manages function pointers for OpenGL. Therefore, we want to initialize it before calling
	// any OpenGL functions.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	glViewport(0, 0, 800, 600); // Set viewport position (bottom left) and size.
	
	// Tell GLFW that we want to call this function when window size changes.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
#pragma endregion

#pragma region Shaders
	// Get current working directory.
	char buff[PATH_MAX];
	_getcwd(buff, PATH_MAX);
	string current_working_dir(buff);
	
	// Create file directory prefixes for shaders.
	const string PREFIX = current_working_dir.substr(0, 37) + "\\Shaders\\";
	
	// Char[] for vertex shader directory.
	char VERTEX_SHADER_DIR[PREFIX.size() + strlen("vertexShader.glsl")];
	strcpy(VERTEX_SHADER_DIR, (PREFIX + "vertexShader.glsl").c_str());
	
	// Char[] for fragment shader directory.
	char FRAGMENT_SHADER_DIR[PREFIX.size() + strlen("fragmentShader.glsl")];
	strcpy(FRAGMENT_SHADER_DIR, (PREFIX + "fragmentShader.glsl").c_str());
	
	// Create shader.
	Shader ourShader(VERTEX_SHADER_DIR, FRAGMENT_SHADER_DIR);
#pragma endregion

#pragma region Textures

#pragma endregion

#pragma region User Defined Shapes
	float vertices[] = {
		// Pos (xyz)       // Col (rgb)
		0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top
		-0.2f, -0.1f, 0.0f, 0.0f, 1.0f, 0.0f, // left
		0.2f, -0.1f, 0.0f, 0.0f, 0.0f, 1.0f, // right
		0.0f, -0.4f, 0.0f, 1.0f, 1.0f, 1.0f  // bottom
	};
	
	unsigned int indices[] = {  // note that we start from 0!
		0, 2, 3,   // front left
		0, 1, 3    // front right
	};
	
	float textureCoordinates[] {
		0.0f, 0.5f, 0.0f, // top
		-0.2f, -0.1f, 0.0f, // left
		0.0f, -0.4f, 0.0f  // bottom
	};
#pragma endregion

#pragma region User Defined Textures
	unsigned int texture;
	glGenTextures(1, &texture); // Set ID to texture.
	glBindTexture(GL_TEXTURE_2D, texture);
	
	// Set texture wrapping and filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use more pixelated filtering to scale down.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Use more blurry filtering to scale up.
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Load image data.
	int width, height, nrChannels;
	unsigned char *data = stbi_load("Textures/container.jpg", &width, &height, &nrChannels, 0);
	
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		cout << " Failed to load texture." << endl;
	}
	
	// Free memory.
	stbi_image_free(data);
#pragma endregion

#pragma region VBO, VBA, EBO
	// Create VBO
	// ----------
	// Vertex buffer object (VBO) - stores a large number of vertices in the GPU's memory.
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	
	// Bind aforementioned buffer to array buffer type. From here on, all buffer calls on the
	// GL_ARRAY_BUFFER target will be used to configure the bound buffer, VBO.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	// Add User Defined Shapes to VBO
	// ------------------------------
	// Copy the triangles vertices into the buffer's memory. glBufferData() is specifically
	// targeted to copy user-defined data into the buffer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// Generate VAO
	// ------------
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO); // Bind VAO
	
	// Copy triangle array in a buffer for OpenGL.
	glBindBuffer(GL_ARRAY_BUFFER, VAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// Set vertex position attribute pointers.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	// Set vertex color attribute pointers.
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	// Generate EBO
	// ------------
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
#pragma endregion
	
	// Render loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window); // Check for key inputs.
		
		// Rendering Commands
		// ------------------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		ourShader.use();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		glfwSwapBuffers(window); // Swaps color buffer.
		glfwPollEvents(); // Checks if any events are triggered.
	}
	
	// Relief buffers.
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	
	// Terminate GLFW and relieve all allocated GLFW resources.
	glfwTerminate();
	return 0;
}