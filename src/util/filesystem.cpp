#include "filesystem.h"

#include <iostream>
#include <glclasses/loader/textureloading.h>
#include <glclasses/loader.h>
#include <glclasses/shader.h>

jnr::FileSystem::FileSystem() : buffersize(2048), buffer(new uint8_t[buffersize]){
    if(!ttvfs::checkCompat())
        std::cerr << "Error: ttvfs abi check!" << std::endl;

    root.AddLoader(new ttvfs::DiskLoader);
}

const void *jnr::FileSystem::readAllBytes(const std::string& path, int* size) {
    ttvfs::File *vf = services::filesystem->root.GetFile(path.c_str());
    if(vf && vf->open("rb")){
        *size = vf->size();
        ensureFit(*size + 1);
        if(vf->read(buffer.get(), *size) != *size) {
            std::cerr << "ERROR: couldn't read all of " << path << std::endl;
            *size = 0;
            return nullptr;
        }
        vf->close();
        return buffer.get();
    }else{
        std::cerr << "ERROR: can't open file " << path << std::endl;
    }
    *size = 0;
    return nullptr;
}

bool jnr::FileSystem::writeAllBytes(const std::string& path, const void* data, int len) {
    ttvfs::File *vf = services::filesystem->root.GetFile(path.c_str());
    if(vf && vf->open("wb")){
        if(vf->write(data, len) != len) {
            std::cerr << "ERROR: couldn't write all of " << path << std::endl;
            return false;
        }
        vf->close();
        return true;
    }else{
        std::cerr << "ERROR: can't open file " << path << std::endl;
    }
    return false;
}

void jnr::FileSystem::ensureFit(int size) {
    if(size <= buffersize)
        return;
    buffersize = size;
    buffer.reset(new uint8_t[buffersize]);
}

std::string jnr::FileSystem::readAllLines(const std::string &path) {
    int size;
    const char* str = static_cast<const char *>(readAllBytes(path, &size));
    buffer.get()[size] = '\0';
    return str;
}

template<>
glc::Program* jnr::FileSystem::readResource<glc::Program>(const std::string &path) {
    return glc::loader::loadResource<glc::Program>(path).release();
}

template<>
glc::Texture* jnr::FileSystem::readResource<glc::Texture>(const std::string &path) {
    return glc::loader::loadTextureFromFile(path);
}

