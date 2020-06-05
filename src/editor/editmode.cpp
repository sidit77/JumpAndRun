#include "editmode.h"
#include "keyhelper.h"

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

float EditMode::getScale() {
    return getCamera().scale / camera::default_scale;
}


void jnr::HitboxDrawMode::render() {
    ImGuiIO& io = ImGui::GetIO();

    vec2 mousepos = toWorldSpace(ImGui::GetMousePos());

    if(!io.WantCaptureMouse && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        clickPos = toWorldSpace(ImGui::GetMousePos());
    }

    if(KeyHelper::isKeyPressed(Key::LEFT_CONTROL) || KeyHelper::isKeyReleased(Key::LEFT_CONTROL))
        clickPos = std::nullopt;
    drawing = KeyHelper::isKeyDown(Key::LEFT_CONTROL);

    if(drawing){
        if(!clickPos){
            vec2 pos = isGridEnabled() ? snapToClosest(mousepos, getGridSpacing()) : mousepos;
            getPrimitveRenderer().drawQuad(pos, vec2(12, 3) * getScale(), +EditorColors::DRAWING_CURSOR, 3.0f, vec2(0.5,0.5));
            getPrimitveRenderer().drawQuad(pos, vec2( 3,12) * getScale(), +EditorColors::DRAWING_CURSOR, 3.0f, vec2(0.5,0.5));
        }else {
            selected.clear();
            vec2 clickedpos = *clickPos;
            vec2 releasepos = mousepos;

            if (isGridEnabled()) {
                clickedpos = snapToGrid(clickedpos, getGridSpacing(), clickedpos - releasepos);
                releasepos = snapToGrid(releasepos, getGridSpacing(), releasepos - clickedpos);
            }
            vec2 low = glm::min(clickedpos, releasepos);
            vec2 high = glm::max(clickedpos, releasepos);

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && low.x - high.x != 0 && low.y - high.y != 0) {
                saveSnapshot();
                getLevel(true).hitboxes.emplace_back(AABB{low, high});
            }

            getPrimitveRenderer().drawAABBOutlineP(low, high, +EditorColors::DRAWING_OUTLINE, 2.5f, 3 * getScale());
            getPrimitveRenderer().drawAABB(low, high, +EditorColors::DRAWING_FILL, 1.5f);
            getPrimitveRenderer().drawQuad(clickedpos, vec2(12, 3) * getScale(), +EditorColors::DRAWING_CURSOR, 3.0f,
                                           vec2(0.5, 0.5));
            getPrimitveRenderer().drawQuad(clickedpos, vec2(3, 12) * getScale(), +EditorColors::DRAWING_CURSOR, 3.0f,
                                           vec2(0.5, 0.5));
            getPrimitveRenderer().drawQuad(releasepos, vec2(12, 3) * getScale(), +EditorColors::DRAWING_CURSOR, 3.0f,
                                           vec2(0.5, 0.5));
            getPrimitveRenderer().drawQuad(releasepos, vec2(3, 12) * getScale(), +EditorColors::DRAWING_CURSOR, 3.0f,
                                           vec2(0.5, 0.5));
        }
    } else {
        if(clickPos){
            vec2 low = glm::min(*clickPos, mousepos);
            vec2 high = glm::max(*clickPos, mousepos);
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                AABB selection(low, high);
                selected.clear();
                for(AABB& aabb : getLevel().hitboxes){
                    if(physics::BoxVsBox(selection, aabb))
                        selected.insert(&aabb);
                }
            }
            getPrimitveRenderer().drawAABB(low, high, +EditorColors::SELECTION_FILL, 1.5f);
        }
    }

    if(ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        clickPos = std::nullopt;

    for(AABB* aabb : selected){
        getPrimitveRenderer().drawAABBOutlineP(aabb->low, aabb->high, +EditorColors::SELECTED_OUTLINE, 2.5f, 3 * max(1.0f, getScale()));
    }

    if(!selected.empty() && KeyHelper::isKeyPressed(Key::DELETE)){
        saveSnapshot();
        std::vector<AABB>& hbs = getLevel(true).hitboxes;
        hbs.erase(std::remove_if(hbs.begin(), hbs.end(), [&selected = selected](AABB& aabb){
            return selected.find(&aabb) != selected.end();
        }), hbs.end());
        selected.clear();
    }

}

void jnr::HitboxDrawMode::onGui() {

}

void jnr::SelectMode::render() {
}

void jnr::SelectMode::onGui() {

}
