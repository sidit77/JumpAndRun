#include "leveleditor.h"

#include <utility>
#include <imgui.h>
#include <iostream>
#include <gtx/string_cast.hpp>
#include <algorithm>
#include <cpp-colors/impl/constants_impl.h>
#include "physics.h"
#include "mixed.h"
#include "gui/guihelper.h"

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

jnr::LevelEditor::LevelEditor(jnr::Config& con, jnr::Camera c, std::shared_ptr<jnr::Level> l, std::shared_ptr<PrimitiveRenderer> pr)
: config(con), cam(c), level(std::move(l)), primitiveRenderer(std::move(pr)) {
    grid = getOrDefault(config["editor"]["grid"], 0);
}

jnr::LevelEditor::~LevelEditor() {
    config["editor"]["grid"] = grid;
    level->rebuildMesh();
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

void jnr::LevelEditor::render(float delta, float catchup, glm::ivec2 screensize) {
    ImGuiIO& io = ImGui::GetIO();
    cam.aspect = (float) screensize.x / screensize.y;
    if (!io.WantCaptureMouse && io.MouseDown[ImGuiMouseButton_Middle])
        cam.position += convert(io.MouseDelta) * vec2(-1, 1) * ((cam.scale * 2) / io.DisplaySize.y);
    if(!ImGui::IsAnyWindowHovered())
        cam.scale = max(cam.scale - io.MouseWheel * 10 * (cam.scale / 200), 10.0f);
    cam.update();

    float spacing = 1024.0f / pow(2, grid);

    if(!io.WantCaptureMouse){
        if(io.MouseClicked[ImGuiMouseButton_Left])
            clickPos[ImGuiMouseButton_Left] = toWorldSpace(cam, io.MousePos);
        if(io.MouseDown[ImGuiMouseButton_Left] || io.MouseReleased[ImGuiMouseButton_Left]){
            vec2 clickedpos = clickPos[ImGuiMouseButton_Left];
            vec2 releasepos = toWorldSpace(cam, io.MousePos);
            if(grid != 0){
                clickedpos = snapToGrid(clickedpos, spacing, clickedpos - releasepos);
                releasepos = snapToGrid(releasepos, spacing, releasepos - clickedpos);
            }
            vec2 low = glm::min(clickedpos, releasepos);
            vec2 high = glm::max(clickedpos, releasepos);

            if(io.MouseReleased[ImGuiMouseButton_Left] && low.x - high.x != 0 && low.y - high.y != 0) {
                level->hitboxes.emplace_back(AABB{glm::min(clickedpos, releasepos), glm::max(clickedpos, releasepos)});
                level->rebuildMesh();
            }

            primitiveRenderer->drawAABB(low, high, colors::colorF(0.3f, 0.42f,0.42f,1.00f), 2.0f);
            primitiveRenderer->drawQuad(clickedpos, vec2(5,5), colors::colorF(0.3f,0.92f,0.42f,1.00f), 2.0f, vec2(0.5,0.5));
            primitiveRenderer->drawQuad(releasepos, vec2(5,5), colors::colorF(0.3f, 0.92f,0.42f,1.00f), 2.0f, vec2(0.5,0.5));

        }
        if(io.MouseReleased[ImGuiMouseButton_Right]){
            vec2 releasepos = toWorldSpace(cam, io.MousePos);
            AABB pointaabb{releasepos, releasepos};
            level->hitboxes.erase(std::remove_if(level->hitboxes.begin(),
                                                 level->hitboxes.end(),
                                                 [&pointaabb](const AABB& aabb){return jnr::AABBCheck(pointaabb, aabb);}), level->hitboxes.end());
        }
    }

    {
        colors::color fill_color(0xF0006EAB);
        colors::color line_color(0xFF00293F);
        for (const AABB &box : level->hitboxes) {
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

bool jnr::LevelEditor::onGui() {
    bool open = true;
    guihelper::beginSaved(config["ui"]["editor"],"Editor", &open, ImGuiWindowFlags_AlwaysAutoResize);
    {
        ImGui::Text("Here are going to be tools!");
        ImGui::InputInt("Grid", &grid, 1, 1);
        grid = max(0, grid);
    }
    ImGui::End();
    return open;
}

jnr::Camera &jnr::LevelEditor::getCam() {
    return cam;
}
