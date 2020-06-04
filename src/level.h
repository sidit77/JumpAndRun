#pragma once

#include <iostream>
#include "mixed.h"
#include "physics.h"

namespace jnr {
    struct LevelHitbox;
}

namespace flatbuffers {
    jnr::LevelHitbox Pack(const jnr::AABB &obj);
    jnr::AABB UnPack(const jnr::LevelHitbox &obj);
}

#include "level_generated.h"

namespace jnr {

    class LevelWrapper : private NonCopyable {
    private:
        LevelT level;
        std::string path;
        bool _hasChanges;
        bool _onDisk;
    public:
        explicit LevelWrapper(std::string  path);
        bool reload();
        bool save();
        LevelT& get(bool mut = false){
            _hasChanges |= mut;
            return level;
        }
        bool onDisk() const{
            return _onDisk;
        }
        bool hasChanges() const{
            return _hasChanges;
        }
    };

}