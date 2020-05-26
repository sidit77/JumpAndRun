#pragma once

#include <glad/glad.h>
#include <string>

namespace jnr{
    class Texture {
    public:
        GLuint id{};
        Texture();
        Texture(const std::string& path);
        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;
        ~Texture();
        void bind(GLenum target, GLenum slot);
    };
}


