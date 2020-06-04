#include "physics.h"
#include <limits>
#include <algorithm>

using namespace jnr;
using namespace glm;

inline AABB move(const AABB& bb, const vec2& pos){
    AABB result{};
    result.low = bb.low + pos;
    result.high = bb.high + pos;
    return result;
}

inline AABB getBroadphaseAABB(const vec2& vel, const AABB& bb){
    AABB result{};
    result.low = bb.low + min(vel, vec2(0,0));
    result.high = bb.high + max(vel, vec2(0,0));
    return result;
}

CollisionInfo getSweptAABB(const AABB& bb1, const vec2& vel, const AABB& bb2){
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
        Entry.x =  -std::numeric_limits<float>::infinity();
        Exit .x =  std::numeric_limits<float>::infinity();
    }else{
        Entry.x = InvEntry.x / vel.x;
        Exit .x = InvExit .x / vel.x;
    }

    if (vel.y == 0.0f){
        Entry.y =  -std::numeric_limits<float>::infinity();
        Exit .y =  std::numeric_limits<float>::infinity();
    }else{
        Entry.y = InvEntry.y / vel.y;
        Exit .y = InvExit .y / vel.y;
    }

    float entryTime = std::max(Entry.x, Entry.y);
    float exitTime  = std::min(Exit .x, Exit .y);
    CollisionInfo result{};
    result.time = entryTime;

    if (entryTime > exitTime || (Entry.x < 0.0f && Entry.y < 0.0f) || Entry.x > 1.0f || Entry.y > 1.0f){
        result.valid = false;
    }else {
        result.valid = true;
        if (Entry.x > Entry.y){
            result.normal = sign(vel) * vec2(-1,0);
            result.position = (vel.x > 0) ? bb2.low.x : bb2.high.x;
        }else{
            result.normal = sign(vel) * vec2(0, -1);
            result.position = (vel.y > 0) ? bb2.low.y : bb2.high.y;
        }
    }
    return result;
}

CollisionInfo jnr::physics::MovingBoxVsBoxes(const glm::vec2& pos, const glm::vec2& vel, const AABB& bb, const std::vector<AABB>& staticbb){
    AABB pbb = move(bb, pos);
    AABB full = getBroadphaseAABB(vel, pbb);
    CollisionInfo info{};
    info.time = std::numeric_limits<float>::infinity();
    info.position = 0;
    info.valid = false;
    float wt = info.time;
    for(const AABB& sbb : staticbb){
        if(BoxVsBox(full, sbb)){
            CollisionInfo ninfo = getSweptAABB(pbb, vel, sbb);
            wt = min(wt, ninfo.time);
            if(ninfo.valid  && ninfo.time < info.time)
                info = ninfo;
        }
    }
    if(!info.valid)
        info.time = wt;
    return info;
}

bool jnr::physics::BoxVsBoxes(const AABB &box, const std::vector<AABB> &boxes) {
    for(const AABB& sbb : boxes){
        if(BoxVsBox(box, sbb)){
            return true;
        }
    }
    return false;
}

bool jnr::physics::BoxVsBoxes(const glm::vec2& pos, const AABB& bb, const std::vector<AABB>& staticbb){
    return BoxVsBoxes(move(bb, pos), staticbb);
}

bool jnr::physics::BoxVsBox(const AABB& bb1, const AABB& bb2){
    return !(bb1.high.x <= bb2.low.x || bb1.low.x >= bb2.high.x || bb1.high.y <= bb2.low.y || bb1.low.y >= bb2.high.y);
}

bool jnr::physics::PointVsBox(const vec2 &p, const AABB &box) {
    return !(p.x <= box.low.x || p.x >= box.high.x || p.y <= box.low.y || p.y >= box.high.y);
}

bool jnr::physics::PointVsBoxes(const vec2 &p, const std::vector<AABB> &boxes) {
    for(const AABB& sbb : boxes){
        if(PointVsBox(p, sbb)){
            return true;
        }
    }
    return false;
}

