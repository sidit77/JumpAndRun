#pragma once

#include <glm.hpp>

namespace jnr {

    namespace camera {
        constexpr float default_scale = 350;
    }

    class Camera{
    public:

        glm::vec2 position;
        float aspect = 1;
        float scale = camera::default_scale;
        glm::mat4 matrix;
        Camera()= default;
        ~Camera()= default;
        void update();
    };
}

