//
// Created by okash on 1/16/2024.
//

#ifndef NOTREALENGINE_ACTOR_H
#define NOTREALENGINE_ACTOR_H

#include "../lib/GLM/glm.hpp"
#include "../lib/GLM/gtc/matrix_transform.hpp"

class actor {
public:
    glm::vec3 Position;
    actor(glm::vec3 pos);
};


#endif //NOTREALENGINE_ACTOR_H