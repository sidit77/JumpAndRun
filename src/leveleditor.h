#pragma once

#include "mixed.h"
#include "rendering/camera.h"
#include "level.h"
#include "config.h"

namespace jnr {
    class LevelEditor : private NonCopyable {
    public:
        jnr::Config& config;
        jnr::Camera cam;
        std::shared_ptr<jnr::Level> level;

        int grid = 0;

        LevelEditor(jnr::Config& config, jnr::Camera cam, std::shared_ptr<jnr::Level> level);
        ~LevelEditor();

        void update(float timestep);
        void render(float delta, float catchup, glm::ivec2 screensize);
        bool onGui();

        jnr::Camera& getCam();
    private:

    };
}

