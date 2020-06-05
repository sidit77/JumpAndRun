#pragma once

#include <cmath>
#include <glm.hpp>

namespace jnr{

    inline float snapToClosest(float v, float space){
        return v - remainder(v, space);
    }

    inline float snapToGrid(float v, float space, bool neg){
        return snapToClosest(v + space * (neg ? -0.5f : 0.5f), space);
    }

    inline glm::vec2 snapToClosest(glm::vec2 v, float space){
        return glm::vec2(snapToClosest(v.x, space),snapToClosest(v.y, space));
    }

    inline glm::vec2 snapToGrid(glm::vec2 v, float space, glm::vec2 dir){
        return glm::vec2(snapToGrid(v.x, space, dir.x < 0),snapToGrid(v.y, space, dir.y < 0));
    }
}