#pragma once

#include <glad/glad.h>
#include <string>
#include "../../mixed.h"

namespace jnr{
    class Texture : private NonCopyable{
    public:
        GLuint id{};
        Texture();
        explicit Texture(const std::string& path);
        ~Texture();
        void bind(GLenum target, GLenum slot);
    };
}


