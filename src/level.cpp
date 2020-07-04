#include "level.h"

#include "util/filesystem.h"

namespace flatbuffers {
    jnr::LevelVec2 convert(const glm::vec2& v){
        return jnr::LevelVec2(v.x, v.y);
    }

    glm::vec2 convert(const jnr::LevelVec2& v){
        return glm::vec2(v.x(), v.y());
    }

    jnr::LevelHitbox Pack(const jnr::AABB& obj){
        return jnr::LevelHitbox(convert(obj.low), convert(obj.high));
    }
    jnr::AABB UnPack(const jnr::LevelHitbox& obj){
        return jnr::AABB(convert(obj.low()), convert(obj.high()));
    }
}

jnr::AABB getPlatform(float x, float y, float w, float h){
    return jnr::AABB{glm::vec2(x,y), glm::vec2(x+w,y+h)};
}

jnr::LevelWrapper::LevelWrapper(std::string  p) : path(std::move(p)) {
    _hasChanges = false;
    _onDisk = false;
    if(!reload()){
        level.name = "Demo";
        level.hitboxes.push_back(getPlatform(0   , 0  , 1500, 10 ));
        level.hitboxes.push_back(getPlatform(1490, 10 , 10  , 700));
        level.hitboxes.push_back(getPlatform(0   , 10 , 10  , 700));
        level.hitboxes.push_back(getPlatform(60  , 500, 200 , 30 ));
        level.hitboxes.push_back(getPlatform(200 , 350, 200 , 30 ));
        level.hitboxes.push_back(getPlatform(300 , 200, 200 , 30 ));
        level.hitboxes.push_back(getPlatform(400 , 600, 200 , 30 ));
        level.hitboxes.push_back(getPlatform(600 , 100, 200 , 30 ));
        level.hitboxes.push_back(getPlatform(650 , 270, 200 , 30 ));
        level.hitboxes.push_back(getPlatform(900 , 400, 200 , 30 ));
        level.hitboxes.push_back(getPlatform(920 , 100, 200 , 30 ));
        level.hitboxes.push_back(getPlatform(1200, 200, 150 , 30 ));
        level.hitboxes.push_back(getPlatform(1340, 300, 10  , 400));
    }
}

bool jnr::LevelWrapper::reload() {
    int size;
    const void* buffer = services::filesystem->readAllBytes(path, &size);
    if(buffer) {
        GetLevel(buffer)->UnPackTo(&level);
        _hasChanges = false;
        _onDisk = true;
        return true;
    }
    return false;
}

bool jnr::LevelWrapper::save() {
    flatbuffers::FlatBufferBuilder fbb;
    fbb.Finish(Level::Pack(fbb, &level));
    if(services::filesystem->writeAllBytes(path, fbb.GetBufferPointer(), fbb.GetSize())){
        _hasChanges = false;
        _onDisk = true;
        return true;
    }
    return false;
}


