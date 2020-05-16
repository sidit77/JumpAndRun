#include <glad/glad.h>
#include <algorithm>
#include "game.h"
#include <imgui.h>

using namespace jnr;

AABB getPlatform(float x, float y, float w, float h){
    return AABB{vec2(x,y), vec2(x+w,y+h)};
}

Game::Game() :
    player(50, 290),
    platforms(),
    program{{"res/shader/character_vertex.glsl", GL_VERTEX_SHADER},{"res/shader/character_fragment.glsl", GL_FRAGMENT_SHADER}},
    vao(),
    vbo()
{
    platforms.push_back(getPlatform(0   , 0  , 1200, 10 ));
    platforms.push_back(getPlatform(1190, 10 , 10  , 700));
    platforms.push_back(getPlatform(0   , 10 , 10  , 700));
    platforms.push_back(getPlatform(60  , 500, 200 , 30 ));
    platforms.push_back(getPlatform(200 , 350, 200 , 30 ));
    platforms.push_back(getPlatform(300 , 200, 200 , 30 ));
    platforms.push_back(getPlatform(400 , 600, 200 , 30 ));
    platforms.push_back(getPlatform(600 , 100, 200 , 30 ));
    platforms.push_back(getPlatform(650 , 270, 200 , 30 ));
    platforms.push_back(getPlatform(900 , 400, 200 , 30 ));
    platforms.push_back(getPlatform(920 , 100, 200 , 30 ));

    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
    };

    vao.bind();
    vbo.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

    player.jump(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);

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
    program.bind();
    vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glUseProgram(0);

    glColor3f(0.0,0.43,0.67f);
    glBegin(GL_QUADS);
    for(const AABB& p : platforms){
        glVertex2f(p.low .x ,p.low .y);
        glVertex2f(p.high.x ,p.low .y);
        glVertex2f(p.high.x ,p.high.y);
        glVertex2f(p.low .x ,p.high.y);
    }
    glEnd();
    glColor3f(1.0f,0.42f,0.42f);
    glBegin(GL_QUADS);
        glVertex2f(player.pos.x + player.vel.x * catchup + player.hitbox.low .x ,player.pos.y + player.vel.y * catchup + player.hitbox.low .y);
        glVertex2f(player.pos.x + player.vel.x * catchup + player.hitbox.high.x ,player.pos.y + player.vel.y * catchup + player.hitbox.low .y);
        glVertex2f(player.pos.x + player.vel.x * catchup + player.hitbox.high.x ,player.pos.y + player.vel.y * catchup + player.hitbox.high.y);
        glVertex2f(player.pos.x + player.vel.x * catchup + player.hitbox.low .x ,player.pos.y + player.vel.y * catchup + player.hitbox.high.y);
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
    ImGui::Text("vel: [%f,%f]", player.vel.x, player.vel.y);
}

