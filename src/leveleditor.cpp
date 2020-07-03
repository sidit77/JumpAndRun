#include "leveleditor.h"

#include <imgui.h>
#include <iostream>
#include <algorithm>
#include "util/mixedmath.h"
#include "editor/keyhelper.h"
#include <GLFW/glfw3.h>
#include "service.h"

using namespace glm;

glm::vec2 convert(ImVec2 v){
    return glm::vec2(v.x, v.y);
}

bool contains(jnr::AABB* a1, std::vector<jnr::AABB*> av){
    for(jnr::AABB* a2 : av){
        if(a1 == a2)
            return true;
    }
    return false;
}

bool KeyButton(const char* label, bool enabled, jnr::MKey key){
    return jnr::guihelper::Button(label, enabled) || (enabled && jnr::KeyHelper::isKeyPressed(key));
}

glm::vec2 jnr::LevelEditorMode::toWorldSpace(ImVec2 v){
    ImGuiIO& io = ImGui::GetIO();
    vec2 clicpos = convert(v);
    clicpos -= convert(io.DisplaySize) / 2.0f;
    clicpos *= vec2(1,-1) * ((cam->scale * 2) / io.DisplaySize.y);
    clicpos += cam->position;
    return clicpos;
}

jnr::LevelEditorMode::LevelEditorMode(){
    auto& config = *jnr::services::config;
    grid = getOrDefault(config["editor"]["grid"], 0);
    editModes.emplace_back(new SelectMode(*this));
    editModes.emplace_back(new HitboxEditMode(*this));

    currentMode = editModes.at(1).get();
    editing = false;
    gridspacing = 0;
}

jnr::LevelEditorMode::~LevelEditorMode() {
    auto& config = *jnr::services::config;
    config["editor"]["grid"] = grid;
    //level->save();
    //level->rebuildMesh();
}

void jnr::LevelEditorMode::snapshot() {
    flatbuffers::FlatBufferBuilder& fbb = undoBuffer.add();
    fbb.Clear();
    fbb.Finish(jnr::Level::Pack(fbb, &level->get(false)));
}

void jnr::LevelEditorMode::restore(bool undo) {
    if((undo && !undoBuffer.canUndo()) || (!undo && !undoBuffer.canRedo()))
        return;
    flatbuffers::FlatBufferBuilder& fbb = undo ? undoBuffer.undo() : undoBuffer.redo();
    GetLevel(fbb.GetBufferPointer())->UnPackTo(&level->get(true));
    for(const std::unique_ptr<EditMode>& mode : editModes)
        mode->wipe();
}

void jnr::LevelEditorMode::update(float timestep) {
    if(!editing)
        PlayMode::update(timestep);
}

void jnr::LevelEditorMode::render(float delta, float catchup) {
    if(!editing){
        PlayMode::render(delta, catchup);
    } else {
        ImGuiIO& io = ImGui::GetIO();
        cam->aspect = (float) io.DisplaySize.x / io.DisplaySize.y;
        if (!io.WantCaptureMouse && ImGui::IsMouseDown(ImGuiMouseButton_Middle))
            cam->position += convert(io.MouseDelta) * vec2(-1, 1) * ((cam->scale * 2) / io.DisplaySize.y);
        if(!ImGui::IsAnyWindowHovered())
            cam->scale = max(cam->scale - io.MouseWheel * 10 * (cam->scale / 200), 10.0f);
        cam->update();

        gridspacing = 1024.0f / pow(2, grid);

        for (const AABB &box : level->get().hitboxes) {
            primitiveRenderer->drawAABB(box.low, box.high, +EditorColors::HITBOX_FILL, 0.0f);
            primitiveRenderer->drawAABBOutlineP(box.low, box.high, +EditorColors::HITBOX_OUTLINE, 0.1f, 2);
        }

        if(grid != 0){
            for(float x = snapToGrid(cam->position.x - cam->scale * cam->aspect, gridspacing, true);
                x <= snapToGrid(cam->position.x + cam->scale * cam->aspect, gridspacing,false); x += gridspacing){
                primitiveRenderer->drawLine(
                        vec2(x, cam->position.y - cam->scale),
                        vec2(x, cam->position.y + cam->scale), +EditorColors::GRID,2);
            }
            for(float y = snapToGrid(cam->position.y - cam->scale, gridspacing, true);
                y <= snapToGrid(cam->position.y + cam->scale, gridspacing,false); y += gridspacing){
                primitiveRenderer->drawLine(
                        vec2(cam->position.x - cam->scale * cam->aspect, y),
                        vec2(cam->position.x + cam->scale * cam->aspect, y), +EditorColors::GRID, 2);
            }
        }

        currentMode->render();

        player->draw(0, 0, *cam);
        if(debugOptions->showPlayerHitbox)
            player->drawDebug(0, 0, *primitiveRenderer);

        primitiveRenderer->render(*cam);
    }
}

void jnr::LevelEditorMode::onGui() {
    auto& config = *jnr::services::config;
    static bool settings = false;
    static bool demo = false;
    guihelper::beginInfoOverlay(config["ui"]["overlay"]);
    {
        ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
        player->ongui(jnr::INFO);
        ImGui::Separator();
        if (ImGui::Button(!editing ? "Open Editor" : "Quit Editor")) {
            editing = !editing;
        }
        ImGui::SameLine();
        if (ImGui::Button("Settings"))
            settings = true;
        ImGui::SameLine();
        if (ImGui::Button("Quit"))
            glfwSetWindowShouldClose(jnr::services::window.get(), GLFW_TRUE);

        if(glfwWindowShouldClose(jnr::services::window.get())){
            ImGui::OpenPopup("AskForSave");
        }

        if (ImGui::BeginPopupModal("AskForSave", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove)){
            ImGui::Text("Save changes?");
            if (ImGui::Button("Yes", ImVec2(60, 30))){
                level->save();
            }
            ImGui::SameLine();
            if (ImGui::Button("No",ImVec2(60, 30))){
                level->reload();
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

            player->ongui(jnr::GRAPHICS);
        }
        if (ImGui::CollapsingHeader("Debug")) {
            ImGui::InputInt("timestep", &debugOptions->timestep, 1, 10);
            debugOptions->timestep = std::max(1, debugOptions->timestep);
            ImGui::SliderFloat("speed", &debugOptions->speed, 0, 3);
            ImGui::Checkbox("movement prediction", &debugOptions->movement_smoothing);
            ImGui::Checkbox("Show Player Hitbox", &debugOptions->showPlayerHitbox);
            if (ImGui::Button("Reset Player")) {
                player->pos = vec2(40, 290);
                player->vel = vec2(0, 0);
                player->force = vec2(0, 0);
                cam->position = vec2(0, 0);
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
        editing = !editing;
    if(editing){
        guihelper::beginSaved(config["ui"]["editor"],"Editor", &editing, ImGuiWindowFlags_AlwaysAutoResize);
        {
            if(KeyButton("Reload", level->hasChanges() && level->onDisk(),MKey::NONE)){
                level->reload();
                undoBuffer.clear();
                for(const std::unique_ptr<EditMode>& mode : editModes)
                    mode->wipe();
            }
            ImGui::SameLine();
            if(KeyButton("Save", level->hasChanges(), MKey::CONTROL | Key::S))
                level->save();
            if(KeyButton("Undo", undoBuffer.canUndo(), MKey::CONTROL | Key::Y)){
                if(!undoBuffer.canRedo()){
                    snapshot();
                    undoBuffer.undo();
                }
                restore(true);
            }
            ImGui::SameLine();
            if(KeyButton("Redo", undoBuffer.canRedo(), MKey::CONTROL | Key::R))
                restore(false);
            ImGui::InputInt("Grid", &grid, 1, 1);
            grid = max(0, grid);
            ImGui::Separator();
            for(const std::unique_ptr<EditMode>& mode : editModes){
                if(KeyButton(mode->getName().c_str(), currentMode != mode.get(), MKey::NONE))
                    currentMode = mode.get();
                ImGui::SameLine();
            }
            ImGui::Dummy(ImVec2(0,0));
            ImGui::Separator();
            currentMode->onGui();
        }
        ImGui::End();
    }
}

bool jnr::LevelEditorMode::canClose() {
    return !level->hasChanges();
}
