#pragma once

#include "physics.h"

namespace jnr {
    struct LevelHitbox;
}

namespace flatbuffers {
    jnr::LevelHitbox Pack(const jnr::AABB &obj);
    jnr::AABB UnPack(const jnr::LevelHitbox &obj);
}

#include "level_generated.h"