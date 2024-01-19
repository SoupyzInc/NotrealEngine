#ifndef NOTREALENGINE_ACTOR_H
#define NOTREALENGINE_ACTOR_H

#include "../lib/GLM/glm.hpp"
#include "../lib/GLM/gtc/matrix_transform.hpp"

class actor {
public:
    glm::vec3 Position;
    explicit actor(glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f));
};


#endif //NOTREALENGINE_ACTOR_H
