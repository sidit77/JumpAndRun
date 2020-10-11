#include "filesystem.h"

#include <iostream>
#include <glclasses/shader.h>
#include <glclasses/texture.h>

jnr::FileSystem::FileSystem() {
    if(!ttvfs::checkCompat())
        std::cerr << "Error: ttvfs abi check!" << std::endl;

    root.AddLoader(new ttvfs::DiskLoader);
}

bool jnr::FileSystem::writeAllBytes(const std::string& path, const void* data, int len) {
    ttvfs::File *vf = root.GetFile(path.c_str());
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

std::string jnr::FileSystem::loadString(const std::string &path) {
    auto bytes = loadBytes(path);
    bytes.push_back('\0');
    return bytes.data();
}

std::vector<char> jnr::FileSystem::loadBytes(const std::string &path) {
    ttvfs::File *vf = root.GetFile(path.c_str());
    if(vf && vf->open("rb")){
        int size = vf->size();
        std::vector<char> result(size + 1);
        result.resize(size);
        bool success = vf->read(result.data(), size) == size;
        vf->close();

        if(success)
            return result;

        std::cerr << "ERROR: couldn't read all of " << path << std::endl;
    }else{
        std::cerr << "ERROR: can't open file " << path << std::endl;
    }
    return std::vector<char>(0);
}

template<>
glc::Program* jnr::FileSystem::readResource<glc::Program>(const std::string &path) {
    return glc::loader::loadResource<glc::Program>(path, *this).release();
}

template<>
glc::Texture* jnr::FileSystem::readResource<glc::Texture>(const std::string &path) {
    return glc::loader::loadResource<glc::Texture>(path, *this).release();
}

