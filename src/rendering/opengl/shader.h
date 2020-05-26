#pragma once

#include <glad/glad.h>
#include <initializer_list>
#include <utility>
#include <string>
#include <memory>

namespace jnr {

    class Shader{
    public:
        GLint id;
        Shader(const std::string&, GLenum);
        Shader(const Shader &) = delete;
        Shader &operator=(const Shader &) = delete;
        ~Shader();
    };

    class Program{
    private:
        GLint id;
    public:
        Program(std::initializer_list<std::shared_ptr<Shader>> shaders);
        Program(const Program &) = delete;
        Program &operator=(const Program &) = delete;
        ~Program();
        void bind();
        GLuint getUniformLocation(std::string name);
    };

}