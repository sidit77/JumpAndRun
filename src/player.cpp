#include <iostream>
#include "player.h"
#include "mixed.h"

using namespace jnr;

const float jump_impulse = 700;

const float gravity = -1200;
const float fall_factor = 2.0f;
const float short_jump_factor = 2.0f;

const float max_speed = 400;
const float ground_friction = 7;
const float ground_acceleration = 1600;
const float ground_reactivity_factor = 3;
const float air_friction = 1;
const float air_acceleration = 2000;
const float air_reactivity_factor = 0.8f;

Player::Player(float x, float y) : hitbox{vec2(-20,0),vec2(20,70)}, foot_hitbox{vec2(-19, -1), vec2(19,1)} {
    pos = vec2(x,y);
    vel = vec2(0,0);
    force = vec2(0,0);
    inair = false;
}

void Player::jump() {
    jumping = true;
    if(inair)
        return;
    vel.y += jump_impulse;
}

void Player::update(float timestep, const std::vector<AABB>& platforms) {
    if(abs(force.x) < 10)
        vel.x -= fabsmin(vel.x * (inair ? air_friction : ground_friction) * timestep, vel.x);
    vel.y += force.y * timestep;

    if(abs(vel.x + force.x * timestep) <= fmax(max_speed, abs(vel.x)))
        vel.x += force.x * timestep;
    else if(abs(vel.x) < max_speed)
        vel.x = clamp(vel.x + force.x * timestep, -max_speed, max_speed);

    if(abs(vel.x) < 1)
        vel.x = 0;

    force = vec2(0, gravity);
    if(vel.y < 0){
        force *= fall_factor;
    } else if (!jumping){
        force *= short_jump_factor;
    }
    jumping = false;
    CollisionInfo info = jnr::checkSweptAABB(pos, vel * timestep, hitbox, platforms);
    if(info.valid){
        pos += vel * timestep * info.time;
        float remainingtime = 1.0f - info.time;
        float dp = (vel.x * info.normal.y + vel.y * info.normal.x);
        vel = vec2(info.normal.y, info.normal.x) * dp;
        pos += vel * timestep * remainingtime;
    } else {
        pos += vel * timestep;
    }
    inair = !jnr::checkAABB(pos, foot_hitbox, platforms);
    //if(vel.y < 0){
    //    for(const Platform& p : platforms){
    //        if(p.y > pos.y)
    //            continue;
    //        if(p.y < pos.y + vel.y * timestep)
    //            break;
    //        if(p.x > pos.x + 20 || p.x + p.w < pos.x - 20)
    //            continue;
    //        inair = false;
    //        pos.y = p.y;
    //        vel.y = 0;
    //    }
    //}



    //force += -vel * 0.2f;
}

void Player::move(float dir) {

    float base = (inair ? air_acceleration : ground_acceleration) * dir;
    if(dir * vel.x < 0.0)
        base *= (inair ? air_reactivity_factor : ground_reactivity_factor);

    //const float tv = 400;
    //float cvp = 1.0f - fmin((vel.x * dir) / tv, 1.0f);
    force.x += base;
}


