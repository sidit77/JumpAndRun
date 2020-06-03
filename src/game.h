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
#include "leveleditor.h"
#include "config.h"
#include "rendering/primitiverenderer.h"

namespace jnr {

    struct DebugOptions {
        int timestep = 144;
        float speed = 1.0f;
        bool movement_smoothing = true;
        bool showPlayerHitbox = false;
    };

    class Game : private NonCopyable{
    private:
        GLFWwindow* window;
        Config& config;
        DebugOptions debugOptions;
        Camera cam;
        Player player;
        std::shared_ptr<PrimitiveRenderer> primitiveRenderer;
        std::shared_ptr<Level> level;
        std::unique_ptr<LevelEditor> editor;
        Input lastInput;
    public:
        Game(Config& c, GLFWwindow* w);
        ~Game();
        void update(float timestep);
        void render(float delta, float catchup, glm::ivec2 screensize);
        void ongui();
        Config& getConfig(){return config;}
        DebugOptions& getDebugOptions(){return debugOptions;}
    };

}



