#pragma once

namespace jnr {
    struct Platform {
        int x, y, w, h;
    };

    inline float fabsmin(float a, float b){
        return abs(a) < abs(b) ? a : b;
    }
}