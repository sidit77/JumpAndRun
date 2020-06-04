#pragma once

#include <cmath>

namespace jnr {

    inline float fabsmin(float a, float b){
        return abs(a) < abs(b) ? a : b;
    }

    inline float faddabs(float a, float b){
        return a < 0 ? a - b : a + b;
    }


}