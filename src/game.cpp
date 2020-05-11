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

