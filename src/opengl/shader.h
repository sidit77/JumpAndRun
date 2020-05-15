#pragma once

#include <glbinding/gl/gl.h>
#include <initializer_list>
#include <utility>
#include <string>

namespace jnr::opengl {

    class Shader{
    public:
        gl::GLint id;
        Shader(const std::string&, gl::GLenum);
        ~Shader();
    };

    class Program{
    private:
        gl::GLint id;
    public:
        Program(std::initializer_list<Shader> shaders);
        ~Program();
        void bind();
    };

}