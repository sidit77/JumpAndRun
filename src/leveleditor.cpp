#include "leveleditor.h"

#include <utility>
#include <imgui.h>
#include <iostream>
#include <gtx/string_cast.hpp>
#include <algorithm>
#include "physics.h"

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

jnr::LevelEditor::LevelEditor(jnr::Camera c, std::shared_ptr<jnr::Level> l) : cam(c), level(std::move(l)) {

}

void jnr::LevelEditor::update(float timestep) {

}

void jnr::LevelEditor::render(float delta, float catchup, glm::ivec2 screensize) {
    ImGuiIO& io = ImGui::GetIO();
    cam.aspect = (float) screensize.x / screensize.y;
    if (!io.WantCaptureMouse && io.MouseDown[ImGuiMouseButton_Middle])
        cam.position += convert(io.MouseDelta) * vec2(-1, 1) * ((cam.scale * 2) / io.DisplaySize.y);
    if(!ImGui::IsAnyWindowHovered())
        cam.scale = max(cam.scale - io.MouseWheel * 10 * (cam.scale / 200), 10.0f);
    cam.update();

    if(!io.WantCaptureMouse){
        if(io.MouseReleased[ImGuiMouseButton_Left]){
            //convert on click
            vec2 clickedpos = toWorldSpace(cam, io.MouseClickedPos[ImGuiMouseButton_Left]);
            vec2 releasepos = toWorldSpace(cam, io.MousePos);

            level->hitboxes.emplace_back(AABB{glm::min(clickedpos, releasepos),glm::max(clickedpos, releasepos)});
            level->rebuildMesh();
        }
        if(io.MouseReleased[ImGuiMouseButton_Right]){
            vec2 releasepos = toWorldSpace(cam, io.MousePos);
            AABB pointaabb{releasepos, releasepos};
            level->hitboxes.erase(std::remove_if(level->hitboxes.begin(),
                                                 level->hitboxes.end(),
                                                 [&pointaabb](const AABB& aabb){return jnr::AABBCheck(pointaabb, aabb);}), level->hitboxes.end());
            level->rebuildMesh();
        }
    }




    level->draw(0, 0, cam);
}

void jnr::LevelEditor::ongui() {
    ImGui::Text("Here are going to be tools!");
}

jnr::Camera &jnr::LevelEditor::getCam() {
    return cam;
}
