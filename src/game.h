#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include "player.h"
#include "physics.h"
#include "opengl/shader.h"
#include "opengl/buffer.h"



namespace jnr {

    class Game {
    private:
        std::vector<AABB> platforms;
        Player player;
        opengl::VertexBuffer vbo;
        opengl::VertexArray vao;
        opengl::Program program;
    public:
        Game();
        ~Game();
        void update(float timestep, GLFWwindow*);
        void render(float catchup);
        void ongui();
    };
}



