#include <direct.h>
#include <climits>
#include <iostream>
#include <cstring>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "lib/GLM/glm.hpp"
#include "lib/GLM/gtc/matrix_transform.hpp"
#include "lib/GLM/gtc/type_ptr.hpp"

#include "Shaders/shader.h"
#include "stb_image.h"

int main() {
	glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	vec = trans * vec;
	std::cout << vec.x << vec.y << vec.z << std::endl;
}