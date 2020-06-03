#pragma once

#include <glm.hpp>
#include <toml/toml.h>

namespace jnr {

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

    template<typename T>
    T getOrDefault(toml::Value& loc, T def, bool write = true){
        if(loc.is<T>())
            return loc.as<T>();
        if(write)
            loc = def;
        return def;
    }

}