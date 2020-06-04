#pragma once

#include "mixed.h"
#include "rendering/camera.h"
#include "rendering/primitiverenderer.h"
#include "level.h"
#include "config.h"

namespace jnr {
    class LevelEditor : private NonCopyable {
    private:
        jnr::Config& config;
        jnr::Camera& cam;
        std::shared_ptr<jnr::LevelWrapper> level;
        std::shared_ptr<jnr::PrimitiveRenderer> primitiveRenderer;

        int grid = 0;
        glm::vec2 clickPos[5];
    public:
        LevelEditor(jnr::Config& config, jnr::Camera& cam, std::shared_ptr<jnr::LevelWrapper> level, std::shared_ptr<jnr::PrimitiveRenderer> pr);
        ~LevelEditor();

        void render(float delta, float catchup, glm::ivec2 screensize);
        bool onGui();

    };


}

