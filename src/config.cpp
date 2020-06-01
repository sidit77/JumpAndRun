#include "config.h"
#include <iostream>
#include <fstream>
#include <utility>

void compareTable(toml::table& a, toml::table& b){
    for(auto&& [k, v] : b){
        v.visit([&c = a, &k = k](auto& node) noexcept{
            if(!c.contains(k) || !c[k].is<decltype(node)>()){
                std::cerr << "Restoring " << k << std::endl;
                c.insert_or_assign(k, std::move(node));
            } else if(toml::is_table<decltype(node)>){
                auto& b = (toml::table&)node;
                compareTable(c[k].ref<toml::table>(), b);
            }
        });
    }
}

jnr::Config::Config(std::string p) : path(std::move(p)){
    toml::table default_config = toml::parse(R"(
        [graphics]
        fullscreen = false
        vsync = 1
        movement_smoothing = true
        timestep = 144
        speed = 1.0
        [display]
        x = 100
        y = 100
        w = 1280
        h = 720
    )");

    std::ifstream myfile (path);
    if (myfile.is_open()){
        try {
            config = toml::parse(myfile);
        } catch (const toml::parse_error& err) {
            std::cerr << "config can't be parsed!" << std::endl;
        }
        myfile.close();
    }else{
        std::cerr << "Config can't be found!" << std::endl;
    }

    compareTable(config, default_config);
}

jnr::Config::~Config() {
    std::ofstream myfile (path);
    if (myfile.is_open())
    {
        myfile << config;
        myfile.close();
    }
    else std::cerr << "cannot save config!" << std::endl;
}
