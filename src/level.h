#pragma once

#include "mixed.h"
#include "physics.h"

namespace jnr {
    class Level : private NonCopyable{
    private:
        std::vector<AABB> hitboxes;
    public:
        Level();
        const std::vector<AABB>& getHitboxes();
    };
}

