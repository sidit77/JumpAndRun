#pragma once

#include <toml/toml.h>
#include "noncopyable.h"

namespace jnr{

    template<typename T>
    T getOrDefault(toml::Value& loc, T def, bool write = true){
        if(loc.is<T>())
            return loc.as<T>();
        if(write)
            loc = def;
        return def;
    }

    class Config : private NonCopyable{
    private:
        std::string path;
        toml::Value config;
        //std::map<std::string, std::map<std::string, std::variant<std::string, int, float>>> data;
    public:
        bool dirty = false;
        explicit Config(std::string path);
        ~Config();
        toml::Value& operator[](const std::string& key);
        void reset();
    };

    template<typename T>
    class ConfigPtr : private NonCopyable{
    private:
        Config& config;
        toml::Value& value;
        T var;
    public:
        ConfigPtr(Config& c, toml::Value& vref) : config(c), value(vref){
            var = vref.as<T>();
        }
        T& getRef(){
            return var;
        }
        ~ConfigPtr(){
            if(value.as<T>() != var){
                config.dirty = true;
                value = var;
            }
        }

    };
}

