#include "level.h"

#include "level_generated.h"

namespace flatbuffers {
    jnr::LevelVec2 convert(const glm::vec2& v){
        return jnr::LevelVec2(v.x, v.y);
    }

    glm::vec2 convert(const jnr::LevelVec2& v){
        return glm::vec2(v.x(), v.y());
    }

    jnr::LevelHitbox Pack(const jnr::AABB& obj){
        return jnr::LevelHitbox(convert(obj.low), convert(obj.high));
    }
    jnr::AABB UnPack(const jnr::LevelHitbox& obj){
        return jnr::AABB(convert(obj.low()), convert(obj.high()));
    }
}
