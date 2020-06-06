#include <iostream>
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

std::optional<AABB> jnr::HitboxEditMode::getGroupAABB(){
    std::optional<AABB> result = std::nullopt;
    for (size_t index : selected) {
        const AABB& aabb = getHitboxes()[index];
        if (!result) {
            result = aabb;
        }
        else {
            result->low = min(result->low, aabb.low);
            result->high = max(result->high, aabb.high);
        }
    }
    return result;
}

std::vector<AABB>& jnr::HitboxEditMode::getHitboxes(bool mut)
{
    return getLevel(mut).hitboxes;
}

void jnr::HitboxEditMode::render() {
    ImGuiIO& io = ImGui::GetIO();

    vec2 mousepos = toWorldSpace(ImGui::GetMousePos());

    while (!selected.empty() && *selected.begin() >= getHitboxes().size())
        selected.erase(selected.begin());

    if(!io.WantCaptureMouse && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        clickPos = toWorldSpace(ImGui::GetMousePos());
        if(interactionMode == InteractionMode::SELECTING) {
            for (size_t index : selected) {
                if (physics::PointVsBox(*clickPos, getHitboxes()[index]))
                    interactionMode = InteractionMode::MOVING;
            }
        }
    }

    if(KeyHelper::isKeyPressed(Key::LEFT_SHIFT)){
        clickPos = std::nullopt;
        interactionMode = InteractionMode::DRAWING;
    }

    switch (interactionMode) {
        case InteractionMode::DRAWING:
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

                if ((KeyHelper::isKeyReleased(Key::LEFT_SHIFT) || ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                && low.x - high.x != 0 && low.y - high.y != 0) {
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
            break;
        case InteractionMode::SELECTING:
            if(clickPos && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                for (size_t i = 0; i < getHitboxes().size(); i++) {
                    if (physics::PointVsBox(*clickPos, getHitboxes()[i])){
                        if(!KeyHelper::isKeyDown(Key::LEFT_CONTROL))
                            selected.clear();
                        selected.insert(i);
                        clickPos = std::nullopt;
                        break;
                    }
                }
            }
            if(clickPos){
                vec2 low = glm::min(*clickPos, mousepos);
                vec2 high = glm::max(*clickPos, mousepos);
                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                    AABB selection(low, high);
                    if(!KeyHelper::isKeyDown(Key::LEFT_CONTROL))
                        selected.clear();
                    for(size_t i = 0; i < getHitboxes().size(); i++){
                        if(physics::BoxVsBox(selection, getHitboxes()[i]))
                            selected.insert(i);
                    }
                }
                getPrimitveRenderer().drawAABB(low, high, +EditorColors::SELECTION_FILL, 1.5f);
            };
            break;
        case InteractionMode::MOVING:
            if(clickPos){
                vec2 delta = mousepos - *clickPos;
                if(isGridEnabled()) {
                    auto group = getGroupAABB();
                    if (group) {
                        vec2 corpos = vec2(delta.x < 0  ? group->low.x : group->high.x, delta.y < 0  ? group->low.y : group->high.y);
                        delta = snapToClosest(corpos + delta, getGridSpacing()) - corpos;
                        getPrimitveRenderer().drawAABBOutlineP(group->low + delta, group->high + delta,
                                +EditorColors::GROUP_MOVE_OUTLINE, 2.4f,3 * max(1.0f, getScale()));
                    }
                }
                for(size_t index : selected){
                    const AABB& aabb = getHitboxes()[index];
                    getPrimitveRenderer().drawAABBOutlineP(aabb.low + delta, aabb.high + delta, +EditorColors::SELECTED_OUTLINE, 2.5f, 3 * max(1.0f, getScale()));
                }
                if(ImGui::IsMouseReleased(ImGuiMouseButton_Left) && length(delta) > 0){
                    saveSnapshot();
                    for (size_t index : selected) {
                        AABB& aabb = getHitboxes(true)[index];
                        aabb.high += delta;
                        aabb.low += delta;
                    }
                }
            }
            break;
    }

    if(KeyHelper::isKeyReleased(Key::LEFT_SHIFT)){
        clickPos = std::nullopt;
        interactionMode = InteractionMode::SELECTING;
    }
    if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
        clickPos = std::nullopt;
        if(interactionMode == InteractionMode::MOVING)
            interactionMode = InteractionMode::SELECTING;
    }


    for (size_t index : selected) {
        const AABB& aabb = getHitboxes()[index];
        getPrimitveRenderer().drawAABBOutlineP(aabb.low, aabb.high, +EditorColors::SELECTED_OUTLINE, 2.5f, 3 * max(1.0f, getScale()));
    }

    if(!selected.empty() && KeyHelper::isKeyPressed(Key::DELETE)){
        saveSnapshot();
        std::vector<AABB>& hbs = getLevel(true).hitboxes;
        for (size_t index : selected) {
            hbs.erase(hbs.begin() + index);
        }
        selected.clear();
    }

    if(!selected.empty() && KeyHelper::isKeyPressed(MKey::CONTROL | Key::D)){
        saveSnapshot();
        size_t start = getHitboxes().size();
        size_t end = selected.size();
        for(size_t index : selected){
            const AABB& aabb = getHitboxes()[index];
            getHitboxes(true).emplace_back(
                    aabb.low + vec2(50, 50) * getScale(),
                    aabb.high + vec2(50, 50) * getScale());
        }
        selected.clear();
        for (size_t index = start; index < start + end; index++) {
            selected.insert(index);
        }
    }

}

void jnr::HitboxEditMode::onGui() {
    ImGui::Value("Mode", (int)interactionMode);
}

void HitboxEditMode::wipe() {
    //selected.clear();
}

void jnr::SelectMode::render() {
}

void jnr::SelectMode::onGui() {

}

void SelectMode::wipe() {

}
