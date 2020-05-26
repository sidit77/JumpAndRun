#include <iostream>
#include "player.h"
#include "mixed.h"
#include <limits>

using namespace jnr;
using namespace glm;

const float jump_impulse = 700;

const float gravity = -1200;
const float fall_factor = 2.0f;
const float short_jump_factor = 3.0f;

const float max_speed = 400;
const float wall_slide_friction = 16;
const float ground_friction = 7;
const float ground_acceleration = 1600;
const float ground_reactivity_factor = 3;
const float air_friction = 1;
const float air_acceleration = 2000;
const float air_reactivity_factor = 0.8f;

Player::Player(float x, float y) :
    hitbox{vec2(-20,0),vec2(20,70)},
    foot_hitbox{vec2(-19, -1), vec2(19,1)},
    l_arm_hitbox{vec2(-21,10),vec2(-19,60)},
    r_arm_hitbox{vec2(19,10),vec2(21,60)}{
    pos = vec2(x,y);
    vel = vec2(0,0);
    force = vec2(0,0);
    inair = false;
}

void Player::jump(bool jump) {
    if(!jump || jumping){
        jumping = jump;
        return;
    }
    jumping = true;
    if(!inair)
        vel.y = jump_impulse;
    else if(onwall)
        vel = vec2((onwall & 1U ? 1 : -1) * 0.7, 1) * jump_impulse;
    else
        return;
    force.y = 0;
    //std::cout << "jump" << std::endl;
}

void Player::update(float timestep, const std::vector<AABB>& platforms) {

    vel.y += force.y * timestep;
    if(abs(vel.x + force.x * timestep) <= fmax(max_speed, abs(vel.x)))
        vel.x += force.x * timestep;
    else if(abs(vel.x) < max_speed)
        vel.x = clamp(vel.x + force.x * timestep, -max_speed, max_speed);

    //jumping = false;
    float remainingtime = timestep;
    //std::cout << "start (" << to_string(vel) << ")";
    for(int i = 0; i < 10 && remainingtime > timestep * 0.05f; i++) {
        CollisionInfo info = jnr::checkSweptAABB(pos, vel * remainingtime, hitbox, platforms);
        if (info.valid) {
            //pos += info.normal * info.depth;
            pos += vel * remainingtime * info.time;
            if(info.normal.x != 0){
                pos.x = info.position - mix(hitbox.low.x, hitbox.high.x, (-info.normal.x + 1) * 0.5f);
            } else {
                pos.y = info.position - mix(hitbox.low.y, hitbox.high.y, (-info.normal.y + 1) * 0.5f);
            }
            remainingtime *= (1-info.time);
            float dp = (vel.x * info.normal.y + vel.y * info.normal.x);
            vel = vec2(info.normal.y, info.normal.x) * dp;
            //std::cout << " > [" << info.time << ", " << info.position << ", " << to_string(info.normal) << ", " << to_string(vel) << "]";
        } else {
            pos += vel * remainingtime;
            remainingtime = 0;
            //std::cout << " > end(" << to_string(pos) << ")";
        }
    }
    //std::cout << " > result(" << jnr::checkAABB(pos, hitbox, platforms) << ")" << std::endl;

    inair = !jnr::checkAABB(pos, foot_hitbox, platforms);
    onwall = 0;
    if(jnr::checkAABB(pos, l_arm_hitbox, platforms))
        onwall |= 1U;
    if(jnr::checkAABB(pos, r_arm_hitbox, platforms))
        onwall |= 2U;

    if(vel.y < 0 && ((onwall & 1U && force.x < 0) || (onwall & 2U && force.x > 0)))
        vel.y -= fabsmin(vel.y * wall_slide_friction * timestep, vel.y);
    if(abs(force.x) < 10)
        vel.x -= fabsmin(vel.x * (inair ? air_friction : ground_friction) * timestep, vel.x);
    if(abs(vel.x) < 1)
        vel.x = 0;

    force = vec2(0, gravity);
    if(vel.y < 0){
        force *= fall_factor;
    } else if (!jumping){
        force *= short_jump_factor;
    }

}

void Player::move(float dir) {

    float base = (inair ? air_acceleration : ground_acceleration) * dir;
    if(dir * vel.x < 0.0)
        base *= (inair ? air_reactivity_factor : ground_reactivity_factor);

    force.x += base;
}


