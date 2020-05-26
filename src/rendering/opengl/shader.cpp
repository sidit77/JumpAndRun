#include "shader.h"
#include <iostream>
#include <ResourceManager/ResourceHandle.h>

using namespace jnr;

Shader::Shader(const std::string& path, GLenum type) : id(glCreateShader(type)) {
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

Shader::~Shader() {
    glDeleteShader(id);
}

Program::Program(std::initializer_list<std::shared_ptr<Shader>> shaders) : id(glCreateProgram()) {
    for(const auto& shader : shaders){
        glAttachShader(id, shader->id);
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

Program::~Program() {
    glDeleteProgram(id);
}

void Program::bind() {
    glUseProgram(id);
}

GLint Program::getUniformLocation(const std::string& name) {
    return glGetUniformLocation(id, name.c_str());
}
