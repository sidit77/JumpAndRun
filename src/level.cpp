#include "level.h"

using namespace jnr;
using namespace glm;

AABB getPlatform(float x, float y, float w, float h){
    return AABB{vec2(x,y), vec2(x+w,y+h)};
}

jnr::Level::Level() : hitboxes(){
    hitboxes.push_back(getPlatform(0   , 0  , 1500, 10 ));
    hitboxes.push_back(getPlatform(1490, 10 , 10  , 700));
    hitboxes.push_back(getPlatform(0   , 10 , 10  , 700));
    hitboxes.push_back(getPlatform(60  , 500, 200 , 30 ));
    hitboxes.push_back(getPlatform(200 , 350, 200 , 30 ));
    hitboxes.push_back(getPlatform(300 , 200, 200 , 30 ));
    hitboxes.push_back(getPlatform(400 , 600, 200 , 30 ));
    hitboxes.push_back(getPlatform(600 , 100, 200 , 30 ));
    hitboxes.push_back(getPlatform(650 , 270, 200 , 30 ));
    hitboxes.push_back(getPlatform(900 , 400, 200 , 30 ));
    hitboxes.push_back(getPlatform(920 , 100, 200 , 30 ));
    hitboxes.push_back(getPlatform(1200, 200, 150 , 30 ));
    hitboxes.push_back(getPlatform(1340, 300, 10  , 400));
}

const std::vector<AABB> &jnr::Level::getHitboxes() {
    return hitboxes;
}
