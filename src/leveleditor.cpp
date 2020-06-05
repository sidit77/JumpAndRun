#include "leveleditor.h"

#include <imgui.h>
#include <iostream>
#include <algorithm>
#include "util/mixedmath.h"
#include "util/guihelper.h"
#include "editor/keyhelper.h"

using namespace glm;

glm::vec2 convert(ImVec2 v){
    return glm::vec2(v.x, v.y);
}

glm::vec2 jnr::LevelEditor::toWorldSpace(ImVec2 v){
    ImGuiIO& io = ImGui::GetIO();
    vec2 clicpos = convert(v);
    clicpos -= convert(io.DisplaySize) / 2.0f;
    clicpos *= vec2(1,-1) * ((cam.scale * 2) / io.DisplaySize.y);
    clicpos += cam.position;
    return clicpos;
}

jnr::LevelEditor::LevelEditor(jnr::Config& con, jnr::Camera& c, std::shared_ptr<jnr::LevelWrapper> l, std::shared_ptr<PrimitiveRenderer> pr)
: config(con), cam(c), level(std::move(l)), primitiveRenderer(std::move(pr)) {
    grid = getOrDefault(config["editor"]["grid"], 0);
    editModes.emplace_back(new SelectMode(*this));
    editModes.emplace_back(new HitboxDrawMode(*this));

    currentMode = editModes.at(1).get();
}

jnr::LevelEditor::~LevelEditor() {
    config["editor"]["grid"] = grid;
    //level->save();
    //level->rebuildMesh();
}

void jnr::LevelEditor::snapshot() {
    flatbuffers::FlatBufferBuilder& fbb = undoBuffer.add();
    fbb.Clear();
    fbb.Finish(jnr::Level::Pack(fbb, &level->get(false)));
}

void jnr::LevelEditor::restore(bool undo) {
    if((undo && !undoBuffer.canUndo()) || (!undo && !undoBuffer.canRedo()))
        return;
    flatbuffers::FlatBufferBuilder& fbb = undo ? undoBuffer.undo() : undoBuffer.redo();
    GetLevel(fbb.GetBufferPointer())->UnPackTo(&level->get(true));
}

bool contains(jnr::AABB* a1, std::vector<jnr::AABB*> av){
    for(jnr::AABB* a2 : av){
        if(a1 == a2)
            return true;
    }
    return false;
}

void jnr::LevelEditor::render(float delta, float catchup, glm::ivec2 screensize) {
    ImGuiIO& io = ImGui::GetIO();
    cam.aspect = (float) screensize.x / screensize.y;
    if (!io.WantCaptureMouse && ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        cam.position += convert(io.MouseDelta) * vec2(-1, 1) * ((cam.scale * 2) / io.DisplaySize.y);
    if(!ImGui::IsAnyWindowHovered())
        cam.scale = max(cam.scale - io.MouseWheel * 10 * (cam.scale / 200), 10.0f);
    cam.update();

    gridspacing = 1024.0f / pow(2, grid);

    for (const AABB &box : level->get().hitboxes) {
        primitiveRenderer->drawAABB(box.low, box.high, +EditorColors::HITBOX_FILL, 0.0f);
        primitiveRenderer->drawAABBOutlineP(box.low, box.high, +EditorColors::HITBOX_OUTLINE, 0.1f, 2);
    }

    if(grid != 0){
        for(float x = snapToGrid(cam.position.x - cam.scale * cam.aspect, gridspacing, true);
            x <= snapToGrid(cam.position.x + cam.scale * cam.aspect, gridspacing,false); x += gridspacing){
            primitiveRenderer->drawLine(
                    vec2(x, cam.position.y - cam.scale),
                    vec2(x, cam.position.y + cam.scale), +EditorColors::GRID,2);
        }
        for(float y = snapToGrid(cam.position.y - cam.scale, gridspacing, true);
            y <= snapToGrid(cam.position.y + cam.scale, gridspacing,false); y += gridspacing){
            primitiveRenderer->drawLine(
                    vec2(cam.position.x - cam.scale * cam.aspect, y),
                    vec2(cam.position.x + cam.scale * cam.aspect, y), +EditorColors::GRID, 2);
        }
    }

    currentMode->render();

}

bool KeyButton(const char* label, bool enabled, jnr::MKey key){
    return jnr::guihelper::Button(label, enabled) || (enabled && jnr::KeyHelper::isKeyPressed(key));
}

bool jnr::LevelEditor::onGui() {
    bool open = true;
    guihelper::beginSaved(config["ui"]["editor"],"Editor", &open, ImGuiWindowFlags_AlwaysAutoResize);
    {
        ImGui::Value("Key", KeyHelper::isKeyDown(MKey::NONE));
        ImGui::Text("Here are going to be tools!");
        if(KeyButton("Reload", level->hasChanges() && level->onDisk(),MKey::NONE)){
            level->reload();
            undoBuffer.clear();
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
        }
    }
    ImGui::End();
    return open;
}
