#include "config.h"
#include <iostream>
#include <fstream>
#include <utility>

toml::Value getDefaultConfig(){
    toml::Value result;
    result["display"]["fullscreen"] = false;
    result["display"]["vsync"] = 1;
    result["display"]["x"] = 100;
    result["display"]["y"] = 100;
    result["display"]["w"] = 1280;
    result["display"]["h"] = 720;

    result["ui"]["overlay"]["corner"] = 0;
    result["ui"]["overlay"]["x"] = 10.0;
    result["ui"]["overlay"]["y"] = 10.0;
    return result;
}

jnr::Config::Config(std::string p) : path(std::move(p)){
    config = getDefaultConfig();

    std::ifstream ifs("config.toml");
    toml::ParseResult pr = toml::parse(ifs);
    ifs.close();
    if (!pr.valid()) {
        std::cout << pr.errorReason << std::endl;
    } else if(!config.merge(pr.value)){
        std::cout << "cannot apply custom config!" << std::endl;
        config = getDefaultConfig();
    }
}

jnr::Config::~Config() {
    std::ofstream myfile (path);
    if (myfile.is_open())
    {
        config.writeFormatted(&myfile, toml::FormatFlag::FORMAT_INDENT);
        myfile.close();
    }
    else std::cerr << "cannot save config!" << std::endl;
}

toml::Value& jnr::Config::operator[](const std::string &key) {
    if (!config.valid())
        config = toml::Value((toml::Table()));

    if (toml::Value* v = config.findChild(key))
        return *v;

    return *config.setChild(key, toml::Value());
}

void jnr::Config::reset() {
    config = getDefaultConfig();
    dirty = true;
}

