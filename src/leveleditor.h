#pragma once

#include "mixed.h"
#include "rendering/camera.h"
#include "rendering/primitiverenderer.h"
#include "level.h"
#include "config.h"

namespace jnr {
    class LevelEditor : private NonCopyable {
    public:
        jnr::Config& config;
        jnr::Camera cam;
        std::shared_ptr<jnr::Level> level;
        std::shared_ptr<jnr::PrimitiveRenderer> primitiveRenderer;

        int grid = 0;

        LevelEditor(jnr::Config& config, jnr::Camera cam, std::shared_ptr<jnr::Level> level, std::shared_ptr<jnr::PrimitiveRenderer> pr);
        ~LevelEditor();

        void update(float timestep);
        void render(float delta, float catchup, glm::ivec2 screensize);
        bool onGui();

        jnr::Camera& getCam();
    private:

    };
}

