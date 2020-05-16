#pragma once


#include <glm.hpp>
#include <vector>
#include "physics.h"

namespace jnr {
    class Player {
    public:
        glm::vec2 pos{};
        glm::vec2 vel{};
        glm::vec2 force{};
        bool inair;
        uint8_t  onwall;
        bool jumping{};
        AABB hitbox;
        AABB foot_hitbox;
        AABB l_arm_hitbox;
        AABB r_arm_hitbox;

        Player(float x, float y);

        void jump(bool jump);

        void move(float dir);

        void update(float timestep, const std::vector<jnr::AABB>&);
    };
}