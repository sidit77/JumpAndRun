#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include "player.h"
#include "physics.h"
#include "camera.h"
#include "opengl/shader.h"
#include "opengl/buffer.h"

namespace jnr {

    class Game {
    private:
        std::vector<AABB> platforms;
        Camera cam;
        Player player;
        opengl::VertexBuffer vbo;
        opengl::VertexArray vao;
        opengl::Program program;
    public:
        Game();
        ~Game();
        void update(float timestep, GLFWwindow*);
        void render(float delta, float catchup, glm::ivec2 screensize);
        void ongui();
    };
}



