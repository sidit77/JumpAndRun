#pragma once

#include "mixed.h"
#include "rendering/camera.h"
#include "level.h"

namespace jnr {
    class LevelEditor : private NonCopyable {
    public:
        jnr::Camera cam;
        std::shared_ptr<jnr::Level> level;

        LevelEditor(jnr::Camera cam, std::shared_ptr<jnr::Level> level);

        void update(float timestep);
        void render(float delta, float catchup, glm::ivec2 screensize);
        void ongui();

        jnr::Camera& getCam();
    private:

    };
}

