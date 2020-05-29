#include <glad/glad.h>
#include "game.h"
#include <imgui.h>
#include <iostream>
#include <memory>
#include <MeshBone.h>


using namespace jnr;
using namespace glm;

AABB getPlatform(float x, float y, float w, float h){
    return AABB{vec2(x,y), vec2(x+w,y+h)};
}


Game::Game() :
        cam(),
        player(50, 290, "assets/character/character_data.json", "assets/character/character_atlas.png"),
        platforms(),
        program{std::make_shared<Shader>("res/shader/platform_vertex.glsl", GL_VERTEX_SHADER),std::make_shared<Shader>("res/shader/platform_fragment.glsl", GL_FRAGMENT_SHADER)},
        staticvao(),
        staticvbo(),
        lastInput()
{
    platforms.push_back(getPlatform(0   , 0  , 1500, 10 ));
    platforms.push_back(getPlatform(1490, 10 , 10  , 700));
    platforms.push_back(getPlatform(0   , 10 , 10  , 700));
    platforms.push_back(getPlatform(60  , 500, 200 , 30 ));
    platforms.push_back(getPlatform(200 , 350, 200 , 30 ));
    platforms.push_back(getPlatform(300 , 200, 200 , 30 ));
    platforms.push_back(getPlatform(400 , 600, 200 , 30 ));
    platforms.push_back(getPlatform(600 , 100, 200 , 30 ));
    platforms.push_back(getPlatform(650 , 270, 200 , 30 ));
    platforms.push_back(getPlatform(900 , 400, 200 , 30 ));
    platforms.push_back(getPlatform(920 , 100, 200 , 30 ));
    platforms.push_back(getPlatform(1200   , 200  , 150, 30 ));
    platforms.push_back(getPlatform(1340   , 300  , 10, 400 ));

    std::vector<vec3> vertices = {
            vec3(-0.5f, -0.5f, 0.7f),
            vec3( 0.5f, -0.5f, 0.7f),
            vec3( 0.0f,  0.5f, 0.7f),
    };

    for(auto plat : platforms){
        vertices.emplace_back(plat.low .x, plat.low .y, 0.0f);
        vertices.emplace_back(plat.high.x, plat.low .y, 0.0f);
        vertices.emplace_back(plat.high.x, plat.high.y, 0.0f);
        vertices.emplace_back(plat.high.x, plat.high.y, 0.0f);
        vertices.emplace_back(plat.low .x, plat.high.y, 0.0f);
        vertices.emplace_back(plat.low .x, plat.low .y, 0.0f);
    }

    staticvao.bind();
    staticvbo.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


}

void Game::update(float timestep, GLFWwindow* window) {
    Input input{};
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        input.move.x -= 1.0f;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        input.move.x += 1.0f;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        input.move.y += 1.0f;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        input.move.y -= 1.0f;

    input.jump = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    if(glfwJoystickIsGamepad(GLFW_JOYSTICK_1)){
        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)){
            input.jump |= state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS;

            vec2 mv(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X],state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);
            if(length(mv) > 0.3)
                input.move = mv;
        }
    }

    input.jumpDown = !lastInput.jump && input.jump;
    //static bool bp;
    //if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    //    if (!bp) {
    //        player.vel += 1000.0f * mv;
    //        bp = true;
    //    }
    //} else {
    //    bp = false;
    //}
    lastInput = input;
    player.update(timestep, input, platforms);
}

float f = 0;
void Game::render(float delta, float catchup, glm::ivec2 screensize) {
    cam.aspect = (float)screensize.x / screensize.y;
    cam.position = glm::mix(cam.position, player.pos + player.vel * catchup, glm::clamp(1-pow(0.1f, delta),0.0f, 1.0f));
    cam.update();

    program.bind();
    staticvao.bind();
    glUniform3f(program.getUniformLocation("color"), 1.0f, 0.5f, 0.7f);
    glUniformMatrix4fv(program.getUniformLocation("cam"), 1, GL_FALSE, glm::value_ptr(mat4(1.0f)));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUniform3f(program.getUniformLocation("color"), 0.0f, 0.43f, 0.67f);
    glUniformMatrix4fv(program.getUniformLocation("cam"), 1, GL_FALSE, glm::value_ptr(cam.matrix));
    glDrawArrays(GL_TRIANGLES, 3, 6 * platforms.size());
    glBindVertexArray(0);
    glUseProgram(0);

    if(showphitbox)
        player.drawDebug(delta, catchup, cam);

    player.draw(delta, catchup, cam);

}

Game::~Game() = default;

void Game::ongui() {
    if(ImGui::Button("Reset Player")){
        player.pos = vec2(40, 290);
        player.vel = vec2(0,0);
        player.force = vec2(0,0);
        cam.position = vec2(0,0);
    }
    ImGui::Checkbox("Show Hitbox", &showphitbox);
    player.ongui();
}

