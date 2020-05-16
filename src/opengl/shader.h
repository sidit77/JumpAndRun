#pragma once

#include <glad/glad.h>
#include <initializer_list>
#include <utility>
#include <string>
#include <memory>

namespace jnr::opengl {

    class Shader{
    public:
        GLint id;
        Shader(const std::string&, GLenum);
        ~Shader();
    };

    class Program{
    private:
        GLint id;
    public:
        Program(std::initializer_list<std::shared_ptr<Shader>> shaders);
        ~Program();
        void bind();
    };

}