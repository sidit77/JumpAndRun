#include <optional>
#include <glclasses/loader.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include "spritesheet.h"

using namespace jnr;

using json = nlohmann::json;

template<>
std::optional<SpriteSheet> glc::loader::loadResource<>(const std::string &path, ResourceLoader& loader) {
    auto j = json::parse(loader.loadString(path));

    auto texture = *glc::loader::loadResource<glc::Texture>(loader.relativePath(path, j["meta"]["image"]), loader);

    std::unordered_map<std::string, SpriteSheet::Sprite> sprites;

    auto size = glm::vec2(
            j["meta"]["size"]["w"],
            j["meta"]["size"]["h"]);

    //asset(size == sheet.texture.size)
    std::cout << size.x << " | " << size.y << std::endl;

    for(auto& [name, sprite] : j["frames"].items()){
        sprites.insert(std::make_pair(name, SpriteSheet::Sprite(
            sprite["frame"]["x"],
            sprite["frame"]["y"],
            sprite["frame"]["width"],
            sprite["frame"]["height"],
            size
        )));
    }

    return std::make_optional<SpriteSheet>(SpriteSheet(std::move(texture), sprites));
}
