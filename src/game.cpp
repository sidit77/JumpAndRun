#include <glad/glad.h>
#include "game.h"
#include <imgui.h>
#include <iostream>
#include <memory>
#include <MeshBone.h>

using namespace jnr;
using namespace glm;



Game::Game(Config& c) :
        config(c),
        cam(),
        player(50, 290, "assets/character/character_data.json", "assets/character/character_atlas.png"),
        level(std::make_shared<Level>()),
        lastInput()
{
    player.setLevel(level);
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
    player.update(timestep, input);
}

void Game::render(float delta, float catchup, glm::ivec2 screensize) {
    cam.aspect = (float)screensize.x / screensize.y;
    cam.position = glm::mix(cam.position, player.pos + player.vel * catchup, glm::clamp(1-pow(0.1f, delta),0.0f, 1.0f));
    cam.update();

    level->draw(delta, catchup, cam);

    if(showphitbox)
        player.drawDebug(delta, catchup, cam);

    player.draw(delta, catchup, cam);

}

Game::~Game() = default;

void Game::ongui() {
    ImGui::Begin("General");
    if(ImGui::CollapsingHeader("Info")){
        ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
        player.ongui(jnr::INFO);
    }
    if(ImGui::CollapsingHeader("Graphics")){
        ImGui::InputInt("timestep", &config.timestep, 1, 10);
        config.timestep = std::max(1, config.timestep);
        ImGui::InputInt("Vsync", &config.vsync, 1, 1);
        config.vsync = std::max(0, config.vsync);
        ImGui::SliderFloat("speed", &config.speed, 0, 3);
        ImGui::Checkbox("Fullscreen", &config.fullscreen);
        ImGui::Checkbox("mov. pred.", &config.movint);
        ImGui::Separator();
        player.ongui(jnr::GRAPHICS);
    }
    if(ImGui::CollapsingHeader("Debug")) {
        if (ImGui::Button("Reset Player")) {
            player.pos = vec2(40, 290);
            player.vel = vec2(0, 0);
            player.force = vec2(0, 0);
            cam.position = vec2(0, 0);
        }
        ImGui::Checkbox("Show Player Hitbox", &showphitbox);
        static bool demo = false;
        if(ImGui::Button("Show ImGui Demo"))
            demo = true;
        if(demo)
            ImGui::ShowDemoWindow(&demo);
    }
    ImGui::End();
}

