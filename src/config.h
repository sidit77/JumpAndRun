#pragma once

#include <map>
#include <variant>
#include "mixed.h"

namespace jnr{
    class Config : private NonCopyable{
    private:
        //std::map<std::string, std::map<std::string, std::variant<std::string, int, float>>> data;
    public:
        bool fullscreen = false;
        int vsync = 1;
        bool movint = true;
        int timestep = 144;
        float speed = 1.0f;
    };
}

