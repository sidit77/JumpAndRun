#include "leveleditor.h"

#include <utility>
#include <imgui.h>
#include <iostream>
#include <algorithm>
#include <cpp-colors/impl/constants_impl.h>
#include "util/mixedmath.h"
#include "util/guihelper.h"
#include "editor/keyhelper.h"

using namespace glm;

glm::vec2 convert(ImVec2 v){
    return glm::vec2(v.x, v.y);
}

glm::vec2 toWorldSpace(jnr::Camera& cam, ImVec2 v){
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
}

jnr::LevelEditor::~LevelEditor() {
    config["editor"]["grid"] = grid;
    //level->save();
    //level->rebuildMesh();
}

inline float snapToClosest(float v, float space){
    return v - remainder(v, space);
}

inline float snapToGrid(float v, float space, bool neg){
    return snapToClosest(v + space * (neg ? -0.5f : 0.5f), space);
}

vec2 snapToClosest(vec2 v, float space){
    return vec2(snapToClosest(v.x, space),snapToClosest(v.y, space));
}

vec2 snapToGrid(vec2 v, float space, vec2 dir){
    return vec2(snapToGrid(v.x, space, dir.x < 0),snapToGrid(v.y, space, dir.y < 0));
}

void snapshot(jnr::LevelWrapper& wrapper, flatbuffers::FlatBufferBuilder& fbb){
    fbb.Clear();
    fbb.Finish(jnr::Level::Pack(fbb, &wrapper.get(false)));
}

void restore(jnr::LevelWrapper& wrapper, flatbuffers::FlatBufferBuilder& fbb){
    jnr::GetLevel(fbb.GetBufferPointer())->UnPackTo(&wrapper.get(true));
}

void jnr::LevelEditor::render(float delta, float catchup, glm::ivec2 screensize) {
    ImGuiIO& io = ImGui::GetIO();
    cam.aspect = (float) screensize.x / screensize.y;
    if (!io.WantCaptureMouse && ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        cam.position += convert(io.MouseDelta) * vec2(-1, 1) * ((cam.scale * 2) / io.DisplaySize.y);
    if(!ImGui::IsAnyWindowHovered())
        cam.scale = max(cam.scale - io.MouseWheel * 10 * (cam.scale / 200), 10.0f);
    cam.update();

    float spacing = 1024.0f / pow(2, grid);

    if(!io.WantCaptureMouse){
        if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            clickPos[ImGuiMouseButton_Left] = toWorldSpace(cam, ImGui::GetMousePos());
        if(ImGui::IsMouseDown(ImGuiMouseButton_Left)|| ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
            vec2 clickedpos = clickPos[ImGuiMouseButton_Left];
            vec2 releasepos = toWorldSpace(cam, ImGui::GetMousePos());
            if(grid != 0){
                clickedpos = snapToGrid(clickedpos, spacing, clickedpos - releasepos);
                releasepos = snapToGrid(releasepos, spacing, releasepos - clickedpos);
            }
            vec2 low = glm::min(clickedpos, releasepos);
            vec2 high = glm::max(clickedpos, releasepos);

            if(ImGui::IsMouseReleased(ImGuiMouseButton_Left) && low.x - high.x != 0 && low.y - high.y != 0) {
                snapshot(*level, undoBuffer.add());
                level->get(true).hitboxes.emplace_back(AABB{glm::min(clickedpos, releasepos), glm::max(clickedpos, releasepos)});
                //level->rebuildMesh();
            }

            primitiveRenderer->drawAABB(low, high, colors::colorF(0.3f, 0.42f,0.42f,1.00f), 2.0f);
            primitiveRenderer->drawQuad(clickedpos, vec2(5 * (cam.scale / camera::default_scale)), colors::colorF(0.3f,0.92f,0.42f,1.00f), 2.0f, vec2(0.5,0.5));
            primitiveRenderer->drawQuad(releasepos, vec2(5 * (cam.scale / camera::default_scale)), colors::colorF(0.3f, 0.92f,0.42f,1.00f), 2.0f, vec2(0.5,0.5));

        }
        if(ImGui::IsMouseReleased(ImGuiMouseButton_Right)){
            vec2 releasepos = toWorldSpace(cam, io.MousePos);
            if(physics::PointVsBoxes(releasepos, level->get().hitboxes)){
                snapshot(*level, undoBuffer.add());
                std::vector<AABB>& hbs = level->get(true).hitboxes;
                hbs.erase(std::remove_if(hbs.begin(), hbs.end(), [&releasepos](const AABB& aabb){
                    return physics::PointVsBox(releasepos, aabb);
                }), hbs.end());
            }

        }
    }
    {
        colors::color fill_color(0xF0006EAB);
        colors::color line_color(0xFF00293F);
        for (const AABB &box : level->get().hitboxes) {
            primitiveRenderer->drawAABB(box.low, box.high, fill_color, 0.0f);
            primitiveRenderer->drawAABBOutlineP(box.low, box.high, line_color, 0.1f, 2);
        }
    }

    if(grid != 0){
        colors::color c((uint32)colors::dotnet::known_color::antique_white);
        c.a = 60;
        for(float x = snapToGrid(cam.position.x - cam.scale * cam.aspect, spacing, true);
            x <= snapToGrid(cam.position.x + cam.scale * cam.aspect, spacing,false); x += spacing){
            primitiveRenderer->drawLine(
                    vec2(x, cam.position.y - cam.scale),
                    vec2(x, cam.position.y + cam.scale), c,1.5f);
        }
        for(float y = snapToGrid(cam.position.y - cam.scale, spacing, true);
            y <= snapToGrid(cam.position.y + cam.scale, spacing,false); y += spacing){
            primitiveRenderer->drawLine(
                    vec2(cam.position.x - cam.scale * cam.aspect, y),
                    vec2(cam.position.x + cam.scale * cam.aspect, y), c, 1.5f);
        }
    }

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
                snapshot(*level, undoBuffer.add());
                undoBuffer.undo();
            }
            restore(*level, undoBuffer.undo());
        }
        ImGui::SameLine();
        if(KeyButton("Redo", undoBuffer.canRedo(), MKey::CONTROL | Key::R))
            restore(*level, undoBuffer.redo());
        ImGui::InputInt("Grid", &grid, 1, 1);
        grid = max(0, grid);
    }
    ImGui::End();
    return open;
}
