#include "editmode.h"

using namespace jnr;
using namespace glm;

jnr::PrimitiveRenderer &jnr::EditMode::getPrimitveRenderer() {
    return *editor.primitiveRenderer;
}

jnr::Camera &jnr::EditMode::getCamera() {
    return editor.cam;
}

jnr::LevelT &jnr::EditMode::getLevel(bool mut) {
    return editor.level->get(mut);
}

bool jnr::EditMode::isGridEnabled() {
    return editor.grid != 0;
}

float jnr::EditMode::getGridSpacing() {
    return editor.gridspacing;
}

glm::vec2 jnr::EditMode::toWorldSpace(ImVec2 v) {
    return editor.toWorldSpace(v);
}

void EditMode::saveSnapshot() {
    editor.snapshot();
}


void jnr::HitboxDrawMode::render() {
    ImGuiIO& io = ImGui::GetIO();

    if(!io.WantCaptureMouse){
        if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            clickPos = toWorldSpace(ImGui::GetMousePos());
        }
        if(clickPos && (ImGui::IsMouseDown(ImGuiMouseButton_Left)|| ImGui::IsMouseReleased(ImGuiMouseButton_Left))){
            vec2 clickedpos = *clickPos;
            vec2 releasepos = toWorldSpace(ImGui::GetMousePos());
            if(isGridEnabled()){
                clickedpos = snapToGrid(clickedpos, getGridSpacing(), clickedpos - releasepos);
                releasepos = snapToGrid(releasepos, getGridSpacing(), releasepos - clickedpos);
            }
            vec2 low = glm::min(clickedpos, releasepos);
            vec2 high = glm::max(clickedpos, releasepos);

            if(ImGui::IsMouseReleased(ImGuiMouseButton_Left) && low.x - high.x != 0 && low.y - high.y != 0) {
                saveSnapshot();
                getLevel(true).hitboxes.emplace_back(AABB{glm::min(clickedpos, releasepos), glm::max(clickedpos, releasepos)});
                //level->rebuildMesh();
            }

            getPrimitveRenderer().drawAABB(low, high, colors::colorF(0.3f, 0.42f,0.42f,1.00f), 2.0f);
            getPrimitveRenderer().drawQuad(clickedpos, vec2(5 * (getCamera().scale / camera::default_scale)), colors::colorF(0.3f,0.92f,0.42f,1.00f), 2.0f, vec2(0.5,0.5));
            getPrimitveRenderer().drawQuad(releasepos, vec2(5 * (getCamera().scale / camera::default_scale)), colors::colorF(0.3f, 0.92f,0.42f,1.00f), 2.0f, vec2(0.5,0.5));

        }
        if(ImGui::IsMouseReleased(ImGuiMouseButton_Right)){
            vec2 releasepos = toWorldSpace(io.MousePos);
            if(physics::PointVsBoxes(releasepos, getLevel().hitboxes)){
                saveSnapshot();
                std::vector<AABB>& hbs = getLevel(true).hitboxes;
                hbs.erase(std::remove_if(hbs.begin(), hbs.end(), [&releasepos](const AABB& aabb){
                    return physics::PointVsBox(releasepos, aabb);
                }), hbs.end());
            }

        }
    }

    if(ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        clickPos = std::nullopt;

    for (const AABB &box : getLevel().hitboxes) {
        getPrimitveRenderer().drawAABB(box.low, box.high, +EditorColors::HITBOX_FILL, 0.0f);
        getPrimitveRenderer().drawAABBOutlineP(box.low, box.high, +EditorColors::HITBOX_OUTLINE, 0.1f, 2);
    }

}

void jnr::HitboxDrawMode::onGui() {

}

void jnr::SelectMode::render() {
    for (const AABB &box : getLevel().hitboxes) {
        getPrimitveRenderer().drawAABB(box.low, box.high, +EditorColors::HITBOX_FILL, 0.0f);
        getPrimitveRenderer().drawAABBOutlineP(box.low, box.high, +EditorColors::HITBOX_OUTLINE, 0.1f, 2);
    }
}

void jnr::SelectMode::onGui() {

}
