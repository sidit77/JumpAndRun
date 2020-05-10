#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include <glm.hpp>

using namespace glm;

namespace jnr {

    struct Platform {
        int x, y, w, h;
    };

    class Player {
    public:
        vec2 pos;
        vec2 vel;
        vec2 force;
        bool inair;
        Player(float x, float y);
        void jump();
        void move(float dir);
        void update(float timestep, const std::vector<Platform>&);
    };

    class Game {
    private:
        std::vector<Platform> platforms;
        Player player;
    public:
        Game();
        ~Game();
        void update(float timestep, GLFWwindow*);
        void render(float catchup);
        void ongui();
    };
}



