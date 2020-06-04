#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include "player.h"
#include "level.h"
#include "leveleditor.h"
#include "util/config.h"
#include "rendering/camera.h"
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
        std::shared_ptr<LevelWrapper> level;
        std::unique_ptr<LevelEditor> editor;
        bool editor_open;
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



