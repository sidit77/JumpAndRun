#pragma once

#include <glm.hpp>
#include <vector>

namespace jnr {

    enum CollisionFace{
        UP, DOWN, LEFT, RIGHT
    };

    struct AABB {
        glm::vec2 low;
        glm::vec2 high;
    };

    struct CollisionInfo {
        CollisionFace face;
        glm::vec2 normal;
        float time;
        bool valid;
        float position;
    };

    CollisionInfo checkSweptAABB(glm::vec2 pos, glm::vec2 vel, AABB bb, const std::vector<AABB>& staticbb);
    bool checkAABB(glm::vec2 pos, AABB bb, const std::vector<AABB>& staticbb);
    bool AABBCheck(AABB bb1, AABB bb2);
}