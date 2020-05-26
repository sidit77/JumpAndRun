#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include <CreatureModule.h>
#include "player.h"
#include "physics.h"
#include "rendering/camera.h"
#include "rendering/opengl/shader.h"
#include "rendering/opengl/buffer.h"
#include "rendering/opengl/texture.h"

namespace jnr {

    class Game {
    private:
        std::unique_ptr<CreatureModule::CreatureManager> creature_manager;
        Program creature_program;
        VertexArray creature_vao;
        VertexBuffer creature_pos, creature_col, creature_tex, creature_ind;
        std::unique_ptr<Texture> creature_texture;
        std::vector<AABB> platforms;
        Camera cam;
        Player player;
        VertexBuffer staticvbo;
        VertexArray staticvao;
        Program program;
        bool showphitbox = false;
    public:
        Game();
        ~Game();
        void update(float timestep, GLFWwindow*);
        void render(float delta, float catchup, glm::ivec2 screensize);
        void ongui();
    };

}



