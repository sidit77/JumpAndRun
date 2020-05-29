#pragma once

#include <glm.hpp>

namespace jnr {
    struct Platform {
        int x, y, w, h;
    };

    class NonCopyable{
    protected:
        constexpr NonCopyable() = default;
        ~NonCopyable() = default;
    public:
        NonCopyable( const NonCopyable& ) = delete;
        NonCopyable& operator=( const NonCopyable& ) = delete;
    };

    inline float fabsmin(float a, float b){
        return abs(a) < abs(b) ? a : b;
    }

    inline float faddabs(float a, float b){
        return a < 0 ? a - b : a + b;
    }

    struct Input {
        glm::vec2 move;
        bool jump;
        bool jumpDown;
    };

    enum GuiSection{
        INFO, GRAPHICS, DEBUG
    };


}