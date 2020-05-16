#include <glad/glad.h>
#include "game.h"
#include <imgui.h>
#include <iostream>
#include <gtc/type_ptr.hpp>

using namespace jnr;
using namespace glm;

AABB getPlatform(float x, float y, float w, float h){
    return AABB{vec2(x,y), vec2(x+w,y+h)};
}

Game::Game() :
        cam(),
        player(50, 290),
        platforms(),
        program{std::make_shared<opengl::Shader>("res/shader/platform_vertex.glsl", GL_VERTEX_SHADER),std::make_shared<opengl::Shader>("res/shader/platform_fragment.glsl", GL_FRAGMENT_SHADER)},
        staticvao(),
        staticvbo()
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

    std::vector<vec2> vertices = {
            vec2(-0.5f, -0.5f),
            vec2( 0.5f, -0.5f),
            vec2( 0.0f,  0.5f),
    };

    for(auto plat : platforms){
        vertices.emplace_back(plat.low .x, plat.low .y);
        vertices.emplace_back(plat.high.x, plat.low .y);
        vertices.emplace_back(plat.high.x, plat.high.y);
        vertices.emplace_back(plat.high.x, plat.high.y);
        vertices.emplace_back(plat.low .x, plat.high.y);
        vertices.emplace_back(plat.low .x, plat.low .y);
    }

    staticvao.bind();
    staticvbo.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
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


void Game::render(float delta, float catchup, glm::ivec2 screensize) {
    cam.aspect = (float)screensize.x / screensize.y;
    cam.position = glm::mix(cam.position, player.pos + player.vel * catchup, glm::clamp(1-pow(0.1f, delta),0.0f, 1.0f));
    cam.update();

    mat4 id(1.0f);

    program.bind();
    staticvao.bind();
    glUniform3f(program.getUniformLocation("color"), 1.0f, 0.5f, 0.7f);
    glUniformMatrix4fv(program.getUniformLocation("cam"), 1, false, glm::value_ptr(mat4(1.0f)));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUniform3f(program.getUniformLocation("color"), 0.0f, 0.43f, 0.67f);
    glUniformMatrix4fv(program.getUniformLocation("cam"), 1, false, glm::value_ptr(cam.matrix));
    glDrawArrays(GL_TRIANGLES, 3, 6 * platforms.size());

    glUseProgram(0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(cam.position.x - (cam.scale * cam.aspect), cam.position.x + (cam.scale * cam.aspect), cam.position.y - cam.scale, cam.position.y + cam.scale, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);

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
        cam.position = vec2(0,0);
    }
    ImGui::Text("vel: [%.2f,%.2f]", player.vel.x, player.vel.y);
}

