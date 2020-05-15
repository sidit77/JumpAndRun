#include "shader.h"
#include <iostream>
#include <ResourceManager/ResourceHandle.h>

using namespace gl;

jnr::opengl::Shader::Shader(const std::string& path, GLenum type) : id(glCreateShader(type)) {
    ResourceHandle handle(path);
    if(!handle.isValid()){
        std::cerr << path << " is not a resource!" << std::endl;
    }else{
        const char* source = handle.c_str();
        glShaderSource(id, 1, &source, NULL);
        glCompileShader(id);
        int success;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if(!success){
            char infoLog[512];
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            std::cerr << "Compilation for shader \"" << path << "\" has failed: \n" << infoLog << std::endl;
        }
    }
}

jnr::opengl::Shader::~Shader() {
    glDeleteShader(id);
}

jnr::opengl::Program::Program(std::initializer_list<Shader> shaders) : id(glCreateProgram()) {
    for(auto shader : shaders){
        glAttachShader(id, shader.id);
    }
    glLinkProgram(id);
    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success){
        char infoLog[512];
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        std::cerr << "Linking program has failed: \n" << infoLog << std::endl;
    }
}

jnr::opengl::Program::~Program() {
    glDeleteProgram(id);
}

void jnr::opengl::Program::bind() {
    glUseProgram(id);
}
