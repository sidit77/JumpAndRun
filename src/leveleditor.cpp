#include "leveleditor.h"

#include <utility>
#include <imgui.h>
#include <iostream>
#include <gtx/string_cast.hpp>
#include <algorithm>
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

jnr::LevelEditor::LevelEditor(jnr::Config& con, jnr::Camera c, std::shared_ptr<jnr::Level> l) : config(con), cam(c), level(std::move(l)) {
    grid = getOrDefault(config["editor"]["grid"], 0);
}

jnr::LevelEditor::~LevelEditor() {
    config["editor"]["grid"] = grid;
}

void jnr::LevelEditor::update(float timestep) {

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

    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(cam.position.x - (cam.scale * cam.aspect), cam.position.x + (cam.scale * cam.aspect), cam.position.y - cam.scale, cam.position.y + cam.scale, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);

    float spacing = 1;
    if(grid != 0){
        spacing = 1024.0f / pow(2, grid);
        glColor4f(0.92f,0.92f,0.92f,0.3f);
        glBegin(GL_LINES);
        for(float x = snapToGrid(cam.position.x - cam.scale * cam.aspect, spacing, true);
        x <= snapToGrid(cam.position.x + cam.scale * cam.aspect, spacing,false); x += spacing){
            glVertex3f(x, cam.position.y - cam.scale, 0.3f);
            glVertex3f(x, cam.position.y + cam.scale, 0.3f);
        }
        for(float y = snapToGrid(cam.position.y - cam.scale, spacing, true);
            y <= snapToGrid(cam.position.y + cam.scale, spacing,false); y += spacing){
            glVertex3f(cam.position.x - cam.scale * cam.aspect, y, 0.3f);
            glVertex3f(cam.position.x + cam.scale * cam.aspect, y, 0.3f);
        }
        glEnd();
    }

    if(!io.WantCaptureMouse){
        if(io.MouseDown[ImGuiMouseButton_Left] || io.MouseReleased[ImGuiMouseButton_Left]){
            //convert on click (camera movement)
            vec2 clickedpos = toWorldSpace(cam, io.MouseClickedPos[ImGuiMouseButton_Left]);
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

            glColor4f(0.42f,0.42f,1.00f,0.3f);
            glBegin(GL_QUADS);
            glVertex3f(low.x, low.y, 0.3f);
            glVertex3f(high.x, low.y, 0.3f);
            glVertex3f(high.x, high.y, 0.3f);
            glVertex3f(low.x, high.y, 0.3f);
            glEnd();

            /**
            glColor4f(0.92f,0.42f,1.00f,0.3f);
            glBegin(GL_QUADS);
            glVertex3f(low .x - 5, low .y - 5, 0.3f);
            glVertex3f(low .x + 5, low .y - 5, 0.3f);
            glVertex3f(low .x + 5, low .y + 5, 0.3f);
            glVertex3f(low .x - 5, low .y + 5, 0.3f);
            glVertex3f(high.x - 5, high.y - 5, 0.3f);
            glVertex3f(high.x + 5, high.y - 5, 0.3f);
            glVertex3f(high.x + 5, high.y + 5, 0.3f);
            glVertex3f(high.x - 5, high.y + 5, 0.3f);
            glEnd();
             **/

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
