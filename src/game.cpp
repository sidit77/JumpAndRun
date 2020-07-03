#include <glad/glad.h>
#include "game.h"
#include "editor/keyhelper.h"
#include "service.h"
#include <imgui.h>
#include <iostream>
#include <memory>
#include <MeshBone.h>
#include <algorithm>

using namespace jnr;
using namespace glm;

Game::Game(GLFWwindow* w) :
        debugOptions(),
        window(w),
        cam(),
        player(50, 290, "assets/character/character_data.json", "assets/character/character_atlas.png"),
        primitiveRenderer(std::make_shared<PrimitiveRenderer>()),
        lastInput(),
        level(std::make_shared<LevelWrapper>(getOrDefault<std::string>((*jnr::services::config)["level"]["name"], "assets/levels/level1.dat")))
{
    editor_open = false;
    player.setLevel(level);
}

void Game::update(float timestep) {
    if(editor_open || glfwWindowShouldClose(window))
        return;

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
    lastInput = input;
    player.update(timestep, input);
}

void Game::render(float delta, float catchup, glm::ivec2 screensize) {
    if(!editor_open){
        cam.aspect = (float)screensize.x / screensize.y;
        cam.position = glm::mix(cam.position, player.pos + player.vel * catchup, glm::clamp(1-pow(0.1f, delta),0.0f, 1.0f));
        cam.scale = glm::mix(cam.scale, camera::default_scale, glm::clamp(1-pow(0.1f, delta),0.0f, 1.0f));
        cam.update();

        {
            colors::color fill_color(0xF0006EAB);
            colors::color line_color(0xFF00293F);
            for (const AABB &box : level->get().hitboxes) {
                primitiveRenderer->drawAABB(box.low, box.high, fill_color, 0.0f);
                primitiveRenderer->drawAABBOutlineP(box.low, box.high, line_color, 0.1f, 2);
            }
            primitiveRenderer->drawQuad(vec2(600, 300) + cam.position / 4.0f, vec2(800,300), line_color, -1.0f, vec2(0.5));
        }

        if(debugOptions.showPlayerHitbox)
            player.drawDebug(delta, catchup, *primitiveRenderer);

        primitiveRenderer->render(cam);
        player.draw(delta, catchup, cam);
    } else {
        if(!editor)
            editor = std::make_unique<LevelEditor>(cam, level, primitiveRenderer);

        editor->render(delta, catchup, screensize);
        player.draw(0, 0, cam);
        if(debugOptions.showPlayerHitbox)
            player.drawDebug(0, 0, *primitiveRenderer);

        primitiveRenderer->render(cam);

    }

}

Game::~Game() = default;

void Game::ongui() {
    auto& config = *jnr::services::config;
    static bool settings = false;
    static bool demo = false;
    guihelper::beginInfoOverlay(config["ui"]["overlay"]);
    {
        ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
        player.ongui(jnr::INFO);
        ImGui::Separator();
        if (ImGui::Button(!editor_open ? "Open Editor" : "Quit Editor")) {
            editor_open = !editor_open;
        }
        ImGui::SameLine();
        if (ImGui::Button("Settings"))
            settings = true;
        ImGui::SameLine();
        if (ImGui::Button("Quit"))
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        if(glfwWindowShouldClose(window)){
            if(level->hasChanges())
                ImGui::OpenPopup("AskForSave");
            else
                quited = true;
        }

        if (ImGui::BeginPopupModal("AskForSave", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove)){
            ImGui::Text("Save changes?");
            if (ImGui::Button("Yes", ImVec2(60, 30))){
                level->save();
                quited = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("No",ImVec2(60, 30))){
                quited = true;
            }
            //ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }
    ImGui::End();



    if(settings) {
        guihelper::beginSaved(config["ui"]["settings"], "Settings", &settings, ImGuiWindowFlags_AlwaysAutoResize);
        //ImGui::SetWindowSize(ImVec2(800, 200));
        if (ImGui::CollapsingHeader("Graphics                             ")) {
            ConfigPtr<int> vsync(config, config["display"]["vsync"]);
            ImGui::InputInt("Vsync", &vsync.getRef(), 1, 1);
            vsync.getRef() = std::max(0, vsync.getRef());

            ConfigPtr<bool> fullscreen(config, config["display"]["fullscreen"]);
            ImGui::Checkbox("Fullscreen", &fullscreen.getRef());

            player.ongui(jnr::GRAPHICS);
        }
        if (ImGui::CollapsingHeader("Debug")) {
            ImGui::InputInt("timestep", &debugOptions.timestep, 1, 10);
            debugOptions.timestep = std::max(1, debugOptions.timestep);
            ImGui::SliderFloat("speed", &debugOptions.speed, 0, 3);
            ImGui::Checkbox("movement prediction", &debugOptions.movement_smoothing);
            ImGui::Checkbox("Show Player Hitbox", &debugOptions.showPlayerHitbox);
            if (ImGui::Button("Reset Player")) {
                player.pos = vec2(40, 290);
                player.vel = vec2(0, 0);
                player.force = vec2(0, 0);
                cam.position = vec2(0, 0);
            }
            if (ImGui::Button("Show ImGui Demo"))
                demo = true;
            if (ImGui::Button("Reset Settings")) {
                config.reset();

            }
        }
        ImGui::End();
    }
    if (demo)
        ImGui::ShowDemoWindow(&demo);
    if(KeyHelper::isKeyPressed(Key::F1))
        editor_open = !editor_open;
    if(editor_open && editor){
        editor_open = editor->onGui();
    }
}

