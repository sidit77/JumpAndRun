#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include <CreatureModule.h>
#include "player.h"
#include "physics.h"
#include "camera.h"
#include "opengl/shader.h"
#include "opengl/buffer.h"
#include "opengl/texture.h"

namespace jnr {

    class Game {
    private:
        std::unique_ptr<CreatureModule::CreatureManager> creature_manager;
        opengl::Program creature_program;
        opengl::VertexArray creature_vao;
        opengl::VertexBuffer creature_pos, creature_col, creature_tex, creature_ind;
        std::unique_ptr<opengl::Texture> creature_texture;
        std::vector<AABB> platforms;
        Camera cam;
        Player player;
        opengl::VertexBuffer staticvbo;
        opengl::VertexArray staticvao;
        opengl::Program program;
        bool showphitbox = false;
    public:
        Game();
        ~Game();
        void update(float timestep, GLFWwindow*);
        void render(float delta, float catchup, glm::ivec2 screensize);
        void ongui();
    };
}



