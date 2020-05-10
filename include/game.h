#pragma once

#include <vector>
#include <GLFW/glfw3.h>

namespace jnr {

    struct Platform {
        int x, y, w, h;
    };

    class Player {
    public:
        float posx, posy, vely;
        bool jumping;
        Player(float x, float y);
        void jump();
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



