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
#include "level.h"
#include "config.h"

namespace jnr {

    class Game : private NonCopyable{
    private:
        GLFWwindow* window;
        Config config;
        Camera cam;
        Player player;
        std::shared_ptr<Level> level;
        bool showphitbox = false;
        Input lastInput;
    public:
        Game(GLFWwindow* w);
        ~Game();
        void update(float timestep);
        void render(float delta, float catchup, glm::ivec2 screensize);
        void ongui();
        Config& getConfig(){return config;}
    };

}



