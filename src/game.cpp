#include <glad/glad.h>
#include "game.h"
#include <imgui.h>
#include <iostream>
#include <memory>
#include <MeshBone.h>
#include <algorithm>

using namespace jnr;
using namespace glm;

Game::Game(GLFWwindow* w) :
        config("config.toml"),
        window(w),
        cam(),
        player(50, 290, "assets/character/character_data.json", "assets/character/character_atlas.png"),
        level(std::make_shared<Level>()),
        lastInput()
{
    player.setLevel(level);
}

void Game::update(float timestep) {
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
    static bool settings = false;
    {
        const float DISTANCE = 10.0f;
        static int corner = 0;
        ImGuiIO &io = ImGui::GetIO();
        if (corner != -1) {
            ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE,
                                       (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
            ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        }
        ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
        ImGuiWindowFlags window_flags =
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        if (corner != -1)
            window_flags |= ImGuiWindowFlags_NoMove;
        ImGui::Begin("Info-Overlay", NULL, window_flags);
        {
            ImGui::Text("fps: %.1f                  ", ImGui::GetIO().Framerate);
            player.ongui(jnr::INFO);
            ImGui::Separator();
            if (ImGui::Button("Settings"))
                settings = true;
            ImGui::SameLine();
            if (ImGui::Button("Quit"))
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            if (ImGui::BeginPopupContextWindow()) {
                if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
                if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
                if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
                if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
                if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    if(settings) {
        ImGui::SetNextWindowBgAlpha(0.6f);
        ImGui::Begin("Settings", &settings, ImGuiWindowFlags_AlwaysAutoResize);
        //ImGui::SetWindowSize(ImVec2(800, 200));
        if (ImGui::CollapsingHeader("Graphics                             ")) {
            int timestep = (int)config.get()["graphics"]["timestep"].ref<int64_t>();
            ImGui::InputInt("timestep", &timestep, 1, 10);
            config.get()["graphics"]["timestep"].ref<int64_t>() = std::max(1, timestep);

            int vsync = (int)config.get()["graphics"]["vsync"].ref<int64_t>();
            ImGui::InputInt("Vsync", &vsync, 1, 1);
            config.get()["graphics"]["vsync"].ref<int64_t>() = std::max(0, vsync);

            float speed = (float)config.get()["graphics"]["speed"].ref<double>();
            ImGui::SliderFloat("speed", &speed, 0, 3);
            config.get()["graphics"]["speed"].ref<double>() = speed;

            ImGui::Checkbox("Fullscreen",          &config.get()["graphics"]["fullscreen"]        .ref<bool>());
            ImGui::Checkbox("movement prediction", &config.get()["graphics"]["movement_smoothing"].ref<bool>());
            player.ongui(jnr::GRAPHICS);
        }
        if (ImGui::CollapsingHeader("Debug")) {
            if (ImGui::Button("Reset Player")) {
                player.pos = vec2(40, 290);
                player.vel = vec2(0, 0);
                player.force = vec2(0, 0);
                cam.position = vec2(0, 0);
            }
            ImGui::Checkbox("Show Player Hitbox", &showphitbox);
            static bool demo = false;
            if (ImGui::Button("Show ImGui Demo"))
                demo = true;
            if (demo)
                ImGui::ShowDemoWindow(&demo);
        }
        ImGui::End();
    }
}

