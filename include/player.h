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
        bool jumping{};
        AABB hitbox;
        AABB foot_hitbox;

        Player(float x, float y);

        void jump();

        void move(float dir);

        void update(float timestep, const std::vector<jnr::AABB>&);
    };
}