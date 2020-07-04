#pragma once

#include <memory>
#include <ttvfs.h>
#include "noncopyable.h"

namespace jnr{
    class FileSystem : private NonCopyable{
    private:
        int buffersize;
        std::unique_ptr<uint8_t> buffer;
        void ensureFit(int size);
    public:
        ttvfs::Root root;
        FileSystem();
        const void* readAllBytes(const std::string& path, int* size);
        std::string readAllLines(const std::string& path);
        bool writeAllBytes(const std::string& path, const void* data, int len);
    };

    namespace services {
        inline std::unique_ptr<FileSystem> filesystem;
    }
}


