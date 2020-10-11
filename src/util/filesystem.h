#pragma once

#include <memory>
#include <ttvfs.h>
#include <glclasses/loader.h>
#include "noncopyable.h"

namespace jnr{
class FileSystem : private NonCopyable, public glc::loader::ResourceLoader{
    public:
        ttvfs::Root root;
        FileSystem();
        std::string loadString(const std::string& path) override;
        std::vector<char> loadBytes(const std::string &path) override;
        bool writeAllBytes(const std::string& path, const void* data, int len);

        template<typename T>
        T* readResource(const std::string& path);
    };

    namespace services {
        inline std::unique_ptr<FileSystem> filesystem;
    }
}


