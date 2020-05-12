#pragma once


#include <glm.hpp>
#include <vector>
#include "physics.h"

using namespace glm;

namespace jnr {
    class Player {
    public:
        vec2 pos{};
        vec2 vel{};
        vec2 force{};
        bool inair;
        uint8  onwall;
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