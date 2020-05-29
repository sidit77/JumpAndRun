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
#include "rendering/creature_renderer.h"

namespace jnr {

    class Game : private NonCopyable{
    private:
        std::vector<AABB> platforms;
        Camera cam;
        Player player;
        VertexBuffer staticvbo;
        VertexArray staticvao;
        Program program;
        bool showphitbox = false;
        Input lastInput;
    public:
        Game();
        ~Game();
        void update(float timestep, GLFWwindow*);
        void render(float delta, float catchup, glm::ivec2 screensize);
        void ongui();
    };

}



