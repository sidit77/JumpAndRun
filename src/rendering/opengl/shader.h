#pragma once

#include <glad/glad.h>
#include <initializer_list>
#include <utility>
#include <string>
#include <memory>
#include "../../mixed.h"

namespace jnr {

    class Shader : private NonCopyable{
    public:
        GLuint id;
        Shader(const std::string&, GLenum);
        ~Shader();
    };

    class Program : private NonCopyable{
    private:
        GLuint id;
    public:
        Program(std::initializer_list<std::shared_ptr<Shader>> shaders);
        ~Program();
        void bind();
        GLint getUniformLocation(const std::string& name);
    };

}