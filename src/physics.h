#pragma once

#include <glm.hpp>
#include <vector>

namespace jnr {

    enum class CollisionFace{
        UP, DOWN, LEFT, RIGHT
    };

    struct AABB {
        AABB() = default;
        AABB(const glm::vec2& l, const glm::vec2& h) : low(l), high(h){};
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

    namespace physics {
        CollisionInfo MovingBoxVsBoxes(const glm::vec2& pos, const glm::vec2& vel, const AABB& box, const std::vector<AABB>& boxes);
        bool BoxVsBoxes(const glm::vec2& pos, const AABB& box, const std::vector<AABB>& boxes);
        bool BoxVsBoxes(const AABB& box, const std::vector<AABB>& boxes);
        bool BoxVsBox(const AABB& box1, const AABB& box2);
        bool PointVsBox(const glm::vec2& p, const AABB& box);
        bool PointVsBoxes(const glm::vec2& p, const std::vector<AABB>& boxes);
    }

}