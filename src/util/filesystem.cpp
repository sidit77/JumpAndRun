#include "filesystem.h"

#include <iostream>
#include <glclasses/shader.h>
#include <glclasses/texture.h>

jnr::FileSystem::FileSystem() {
    if(!ttvfs::checkCompat())
        std::cerr << "Error: ttvfs abi check!" << std::endl;

    root.AddLoader(new ttvfs::DiskLoader);
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

bool jnr::FileSystem::writeBytes(const std::string &path, const void* data, size_t len) {
    ttvfs::File *vf = root.GetFile(path.c_str());
    if(vf && vf->open("wb")){
        bool success = vf->write(data, len) == len;
        vf->close();
        if(success)
            return true;
        std::cerr << "ERROR: couldn't write all of " << path << std::endl;
    }else{
        std::cerr << "ERROR: can't open file " << path << std::endl;
    }
    return false;
}

bool jnr::FileSystem::writeString(const std::string &path, const std::string &str) {
    return writeBytes(path, str.data(), str.size());
}

bool jnr::FileSystem::writeBytes(const std::string &path, std::vector<char> bytes) {
    return writeBytes(path, bytes.data(), bytes.size());
}

//template<typename T>
//std::unique_ptr<T> jnr::FileSystem::readResource(const std::string &path) {
//    return glc::loader::loadResource<T>(path, *this);
//}

