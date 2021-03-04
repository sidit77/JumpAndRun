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
        bool writeString(const std::string &path, const std::string &str) override;
        bool writeBytes(const std::string &path, std::vector<char> bytes) override;
        bool writeBytes(const std::string &path, const void* data, size_t len);

        template<typename T>
        std::optional<T> readResource(const std::string& path){
            return glc::loader::loadResource<T>(path, *this);
        }

        template<typename T>
        std::shared_ptr<T> makeResourcePtr(const std::string& path){
            auto res = readResource<T>(path);
            if(res.has_value())
                return std::make_unique<T>(std::move(res.value()));
            return nullptr;
        }
    };

    namespace services {
        inline std::unique_ptr<FileSystem> filesystem;
    }
}


