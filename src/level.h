#pragma once

#include "mixed.h"
#include "physics.h"
#include "rendering/camera.h"
#include "rendering/opengl/buffer.h"
#include "rendering/opengl/shader.h"

namespace jnr {
    class Level : private NonCopyable{
    private:
        std::vector<AABB> hitboxes;
        VertexBuffer staticvbo;
        VertexArray staticvao;
        Program program;
    public:
        Level();
        const std::vector<AABB>& getHitboxes();

        void draw(float delta, float catchup, Camera& cam);
    };
}

