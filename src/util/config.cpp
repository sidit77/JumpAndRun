#include "config.h"
#include <iostream>
#include <fstream>
#include <utility>

jnr::Config::Config(std::string p) : path(std::move(p)), config(){
    std::ifstream ifs(path);
    toml::ParseResult pr = toml::parse(ifs);
    ifs.close();
    if (pr.valid()) {
        config = pr.value;
    } else {
        std::cout << path << ": " << pr.errorReason << std::endl;
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
    std::cout << "Saving..." << std::endl;
}

toml::Value& jnr::Config::operator[](const std::string &key) {
    if (!config.valid())
        config = toml::Value((toml::Table()));

    if (toml::Value* v = config.findChild(key))
        return *v;

    return *config.setChild(key, toml::Value());
}

void jnr::Config::reset() {
    config = toml::Value();
    dirty = true;
}

