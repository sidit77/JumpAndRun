#pragma once

#include <glm.hpp>
#include <vector>

using namespace glm;

namespace jnr {

    enum CollisionFace{
        UP, DOWN, LEFT, RIGHT
    };

    struct AABB {
        vec2 low;
        vec2 high;
    };

    struct CollisionInfo {
        CollisionFace face;
        vec2 normal;
        float time;
        bool valid;
    };

    CollisionInfo checkSweptAABB(vec2 pos, vec2 vel, AABB bb, const std::vector<AABB>& staticbb);
    bool checkAABB(vec2 pos, AABB bb, const std::vector<AABB>& staticbb);
}