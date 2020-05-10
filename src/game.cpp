#include <glad/glad.h>
#include <algorithm>
#include <game.h>
#include <imgui.h>

using namespace jnr;

Game::Game() : player(20, 290), platforms() {
    platforms.push_back(Platform{40 , 60 , 800, 30});
    platforms.push_back(Platform{60 , 500, 200, 30});
    platforms.push_back(Platform{200, 350, 200, 30});
    platforms.push_back(Platform{300, 200, 200, 30});
    platforms.push_back(Platform{400, 600, 200, 30});
    platforms.push_back(Platform{600, 100, 200, 30});
    platforms.push_back(Platform{650, 270, 200, 30});
    platforms.push_back(Platform{900, 400, 200, 30});
    platforms.push_back(Platform{920, 100, 200, 30});
    sort(platforms.begin( ), platforms.end( ), [ ]( const Platform& lhs, const Platform& rhs ){ return lhs.y > rhs.y;});
}

void Game::update(float timestep, GLFWwindow* window) {

    vec2 mv(0,0);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mv.x -= 1.0f;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mv.x += 1.0f;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mv.y += 1.0f;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mv.y -= 1.0f;
    player.move(mv.x);

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        player.jump();

    static bool bp;
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (!bp) {
            player.vel += 1000.0f * mv;
            bp = true;
        }
    } else {
        bp = false;
    }
    player.update(timestep, platforms);
}

void Game::render(float catchup) {
    glColor3f(0.0,0.43,0.67f);
    glBegin(GL_QUADS);
    for(const Platform& p : platforms){
        glVertex2f(p.x    ,p.y    );
        glVertex2f(p.x+p.w,p.y    );
        glVertex2f(p.x+p.w,p.y-p.h);
        glVertex2f(p.x    ,p.y-p.h);
    }
    glEnd();
    glColor3f(1.0f,0.42f,0.42f);
    glBegin(GL_QUADS);
        glVertex2f(player.pos.x + player.vel.x * catchup - 20 ,player.pos.y + player.vel.y * catchup     );
        glVertex2f(player.pos.x + player.vel.x * catchup + 20 ,player.pos.y + player.vel.y * catchup     );
        glVertex2f(player.pos.x + player.vel.x * catchup + 20 ,player.pos.y + player.vel.y * catchup +70);
        glVertex2f(player.pos.x + player.vel.x * catchup - 20 ,player.pos.y + player.vel.y * catchup +70);
    glEnd();
}

Game::~Game() {

}

void Game::ongui() {
    if(ImGui::Button("Reset Player")){
        player.pos = vec2(40, 290);
        player.vel = vec2(0,0);
        player.force = vec2(0,0);
    }
    ImGui::Text("x-vel: %f", player.vel.x);
}

Player::Player(float x, float y) {
    pos = vec2(x,y);
    vel = vec2(0,0);
    force = vec2(0,0);
    inair = false;
}

void Player::jump() {
    jumping = true;
    if(inair)
        return;
    vel.y = 700;
}

void Player::update(float timestep, const std::vector<Platform>& platforms) {
    if(!inair && abs(force.x) < 10)
        vel.x += -vel.x * 7.0f * timestep;
    vel += force * timestep;
    if(abs(vel.x) < 1)
        vel.x = 0;
    inair = true;
    force = vec2(0, -1200);
    if(vel.y < 0){
        force *= 2.0f;
    } else if (!jumping){
        force *= 2.0f;
    }
    jumping = false;
    if(vel.y < 0){
        for(const Platform& p : platforms){
            if(p.y > pos.y)
                continue;
            if(p.y < pos.y + vel.y * timestep)
                break;
            if(p.x > pos.x + 20 || p.x + p.w < pos.x - 20)
                continue;
            inair = false;
            pos.y = p.y;
            vel.y = 0;
        }
    }
    pos += vel * timestep;

    //force += -vel * 0.2f;
}

void Player::move(float dir) {
    const float tv = 400;
    float cvp = 1.0f - fmin((vel.x * dir) / tv, 1.0f);
    force.x += 3000 * dir * cvp;
}
