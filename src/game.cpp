#include <glad/glad.h>
#include <algorithm>
#include <game.h>

using namespace jnr;

Game::Game() : player(20, 290), platforms() {
    platforms.push_back(Platform{40 , 60 , 200, 30});
    platforms.push_back(Platform{60 , 500, 200, 30});
    platforms.push_back(Platform{200, 350, 200, 30});
    platforms.push_back(Platform{300, 200, 200, 30});
    platforms.push_back(Platform{400, 600, 200, 30});
    platforms.push_back(Platform{600, 70 , 200, 30});
    platforms.push_back(Platform{650, 270, 200, 30});
    platforms.push_back(Platform{900, 400, 200, 30});
    platforms.push_back(Platform{920, 100, 200, 30});
    sort(platforms.begin( ), platforms.end( ), [ ]( const Platform& lhs, const Platform& rhs ){ return lhs.y > rhs.y;});
}

void Game::update(float timestep, GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        player.posx -=5;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        player.posx +=5;
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        player.jump();
    player.update(timestep, platforms);
}

void Game::render() {
    glColor3f(0.0,0.0,1.0f);
    glBegin(GL_QUADS);
    for(const Platform& p : platforms){
        glVertex2f(p.x    ,p.y    );
        glVertex2f(p.x+p.w,p.y    );
        glVertex2f(p.x+p.w,p.y-p.h);
        glVertex2f(p.x    ,p.y-p.h);
    }
    glEnd();
    glColor3f(1.0f,0.0,0.0);
    glBegin(GL_QUADS);
        glVertex2f(player.posx - 20 ,player.posy    );
        glVertex2f(player.posx + 20 ,player.posy    );
        glVertex2f(player.posx + 20 ,player.posy+70);
        glVertex2f(player.posx - 20 ,player.posy+70);
    glEnd();
}

Game::~Game() {

}

Player::Player(float x, float y) {
    posx = x;
    posy = y;
    vely = 0;
    jumping = false;
}

void Player::jump() {
    if(jumping)
        return;
    jumping = true;
    vely = 15;
}

void Player::update(float timestep, const std::vector<Platform>& platforms) {
    if(vely < 0){
        for(const Platform& p : platforms){
            if(p.y > posy)
                continue;
            if(p.y < posy + vely)
                break;
            if(p.x > posx + 20 || p.x + p.w < posx - 20)
                continue;
            jumping = false;
            posy = p.y;
            vely = 0;
        }
    }
    posy += vely;
    vely -= 0.5f;
}
