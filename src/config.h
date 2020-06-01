#pragma once

#include <toml++/toml.h>
#include "mixed.h"

namespace jnr{
    class Config : private NonCopyable{
    private:
        std::string path;
        toml::table config;
        //std::map<std::string, std::map<std::string, std::variant<std::string, int, float>>> data;
    public:
        Config(std::string path);
        ~Config();
        toml::table& get() { return config; }
    };
}

