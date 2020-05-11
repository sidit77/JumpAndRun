#include "physics.h"
#include <limits>
#include <algorithm>
#include <gtx/string_cast.hpp>

using namespace jnr;

inline AABB move(AABB bb, vec2 pos){
    AABB result;
    result.low = bb.low + pos;
    result.high = bb.high + pos;
    return result;
}

inline AABB getBroadphaseAABB(vec2 vel, AABB bb){
    AABB result;
    result.low = bb.low + min(vel, vec2(0,0));
    result.high = bb.high + max(vel, vec2(0,0));
    return result;
}

inline bool AABBCheck(AABB bb1, AABB bb2){
    return !(bb1.high.x < bb2.low.x || bb1.low.x > bb2.high.x || bb1.high.y < bb2.low.y || bb1.low.y > bb2.high.y);
}

CollisionInfo getSweptAABB(AABB bb1, vec2 vel, AABB bb2){
    vec2 InvEntry, InvExit;

    if (vel.x > 0.0f){
        InvEntry.x = bb2.low .x - bb1.high.x;
        InvExit .x = bb2.high.x - bb1.low .x;
    }else{
        InvEntry.x = bb2.high.x - bb1.low .x;
        InvExit .x = bb2.low .x - bb1.high.x;
    }

    if (vel.y > 0.0f){
        InvEntry.y = bb2.low .y - bb1.high.y;
        InvExit .y = bb2.high.y - bb1.low .y;
    }else{
        InvEntry.y = bb2.high.y - bb1.low .y;
        InvExit .y = bb2.low .y - bb1.high.y;
    }

    vec2 Entry, Exit;
    if (vel.x == 0.0f){
        Entry.x = -std::numeric_limits<float>::infinity();
        Exit .x =  std::numeric_limits<float>::infinity();
    }else{
        Entry.x = InvEntry.x / vel.x;
        Exit .x = InvExit .x / vel.x;
    }

    if (vel.y == 0.0f){
        Entry.y = -std::numeric_limits<float>::infinity();
        Exit .y = std::numeric_limits<float>::infinity();
    }else{
        Entry.y = InvEntry.y / vel.y;
        Exit .y = InvExit .y / vel.y;
    }

    float entryTime = std::max(Entry.x, Entry.y);
    float exitTime  = std::min(Exit .x, Exit .y);
    CollisionInfo result{};

    if (entryTime > exitTime || (Entry.x < 0.0f && Entry.y < 0.0f) || Entry.x > 1.0f || Entry.y > 1.0f){
        result.valid = false;
    }else {
        result.valid = true;
        result.time = entryTime;
        if (Entry.x > Entry.y){
            if (InvEntry.x < 0.0f){
                result.normal.x = 1.0f;
                result.normal.y = 0.0f;
                result.face = RIGHT;
            }else{
                result.normal.x = -1.0f;
                result.normal.y = 0.0f;
                result.face = LEFT;
            }
        }else{
            if (InvEntry.y < 0.0f){
                result.normal.x = 0.0f;
                result.normal.y = 1.0f;
                result.face = UP;
            }else{
                result.normal.x = 0.0f;
                result.normal.y = -1.0f;
                result.face = DOWN;
            }
        }
    }
    return result;
}

CollisionInfo jnr::checkSweptAABB(vec2 pos, vec2 vel, AABB bb, const std::vector<AABB>& staticbb){
    AABB pbb = move(bb, pos);
    AABB full = getBroadphaseAABB(vel, pbb);
    CollisionInfo info{};
    info.time = 2.0f;
    info.valid = false;
    for(const AABB& sbb : staticbb){
        if(AABBCheck(full, sbb)){
            CollisionInfo ninfo = getSweptAABB(pbb, vel, sbb);
            if(ninfo.valid && ninfo.time < info.time)
                info = ninfo;
        }
    }
    return info;
}

bool jnr::checkAABB(vec2 pos, AABB bb, const std::vector<AABB>& staticbb){
    AABB pbb = move(bb, pos);
    for(const AABB& sbb : staticbb){
        if(AABBCheck(pbb, sbb)){
            return true;
        }
    }
    return false;
}

