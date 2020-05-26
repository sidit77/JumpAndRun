#pragma once

#include <glm.hpp>

namespace jnr {
    class Camera{
    public:
        glm::vec2 position;
        float aspect = 1;
        float scale = 350;
        glm::mat4 matrix;
        Camera()= default;
        ~Camera()= default;
        void update();
    };
}

