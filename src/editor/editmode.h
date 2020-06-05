#pragma once

#include <string>
#include "../util/noncopyable.h"
#include "../leveleditor.h"
#include "editormath.h"

namespace jnr {

    class LevelEditor;

    class EditMode : private NonCopyable{
    protected:
        std::string name;
        jnr::LevelEditor& editor;

        PrimitiveRenderer& getPrimitveRenderer();
        Camera& getCamera();
        LevelT& getLevel(bool mut = false);
        bool isGridEnabled();
        float getGridSpacing();
        glm::vec2 toWorldSpace(ImVec2 v);
        void saveSnapshot();
        float getScale();
    public:
        EditMode(jnr::LevelEditor& e, std::string n) : editor(e), name(std::move(n)){}
        const std::string& getName(){return name;}
        virtual void render() = 0;
        virtual void onGui() = 0;
    };

    class HitboxDrawMode : public EditMode {
    private:
        std::optional<glm::vec2> clickPos;
        bool drawing;
    public:
        HitboxDrawMode(LevelEditor& e) : EditMode(e, "Draw Hitbox") {};
        void render() override;
        void onGui() override;
    };

    class SelectMode : public EditMode {
    public:
        SelectMode(LevelEditor& e) : EditMode(e, "Select") {};
    private:
        void render() override;
        void onGui() override;
    };

}

