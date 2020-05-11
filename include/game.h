#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include "player.h"

using namespace glm;

namespace jnr {

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



