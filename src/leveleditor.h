#pragma once

#include <imgui.h>
#include "rendering/camera.h"
#include "rendering/primitiverenderer.h"
#include "level.h"
#include "util/config.h"
#include "util/undobuffer.h"
#include "editor/editmode.h"

namespace jnr {

    enum class EditorColors : uint32_t {
        GRID = 0x3FFAFAFA,
        HITBOX_OUTLINE = 0xFF00293F,
        HITBOX_FILL = 0xF0006EAB,
        DRAWING_CURSOR = 0xFA1A1A1A,
        DRAWING_OUTLINE = 0xAA4A4A4A,
        DRAWING_FILL = 0x444A4A4A,
        SELECTION_FILL = 0x44AAAAFA,
        SELECTED_OUTLINE = 0xFFEB7134,
        GROUP_MOVE_OUTLINE = 0xAAEB7134,
        RESIZE_HOVER_OVERLAY = 0x99AAAAAA
    };

    inline colors::color operator+(const enum EditorColors ec) {
        return colors::color((uint32_t)ec);
    }

    class EditMode;

    class LevelEditor : private NonCopyable {
    private:
        jnr::Config& config;
        jnr::Camera& cam;
        jnr::UndoBuffer<flatbuffers::FlatBufferBuilder, 10> undoBuffer;
        std::shared_ptr<jnr::LevelWrapper> level;
        std::shared_ptr<jnr::PrimitiveRenderer> primitiveRenderer;

        int grid = 0;
        float gridspacing;
        std::vector<std::unique_ptr<jnr::EditMode>> editModes;
        jnr::EditMode* currentMode;

        glm::vec2 toWorldSpace(ImVec2 v);
        void snapshot();
        void restore(bool undo);
    public:
        friend class EditMode;
        LevelEditor(jnr::Config& config, jnr::Camera& cam, std::shared_ptr<jnr::LevelWrapper> level, std::shared_ptr<jnr::PrimitiveRenderer> pr);
        ~LevelEditor();

        void render(float delta, float catchup, glm::ivec2 screensize);
        bool onGui();

    };


}

