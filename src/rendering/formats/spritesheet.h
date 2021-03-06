#pragma once

#include <glm.hpp>
#include <utility>
#include <glclasses/texture.h>
#include <unordered_map>

namespace jnr {

    // : private NonCopyable
    class SpriteSheet {
    public:
        struct Sprite {
            glm::vec2 low;
            glm::vec2 high;
            Sprite(float x, float y, float w, float h, glm::vec2 size) :
                low(glm::vec2(x,y + h) / size),
                high(glm::vec2(x + w,y) / size){}
        };
        std::unordered_map<std::string, Sprite> sprites;
        glc::Texture texture;
        explicit SpriteSheet(glc::Texture tex, std::unordered_map<std::string, Sprite> spri) : texture(std::move(tex)), sprites(std::move(spri)) {}
        SpriteSheet(SpriteSheet&& other) noexcept : SpriteSheet(std::move(other.texture), std::move(other.sprites))  {}

        SpriteSheet& operator=(SpriteSheet&& other) noexcept {
            if (this == &other)
                return *this;
            sprites = std::move(other.sprites);
            texture = std::move(other.texture);
            return *this;
        }
        ~SpriteSheet() = default;
    };
}


