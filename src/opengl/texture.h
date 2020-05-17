#pragma once

#include <glad/glad.h>
#include <string>

namespace jnr::opengl{
    class Texture {
    public:
        GLuint id;
        Texture();
        Texture(std::string path);
        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;
        ~Texture();
        void bind(GLenum target, GLenum slot);
    };
}


