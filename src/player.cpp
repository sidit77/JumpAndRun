#include "player.h"

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

Player::Player(float x, float y) {
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

void Player::update(float timestep, const std::vector<Platform>& platforms) {
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
    inair = true;
    jumping = false;
    if(vel.y < 0){
        for(const Platform& p : platforms){
            if(p.y > pos.y)
                continue;
            if(p.y < pos.y + vel.y * timestep)
                break;
            if(p.x > pos.x + 20 || p.x + p.w < pos.x - 20)
                continue;
            inair = false;
            pos.y = p.y;
            vel.y = 0;
        }
    }
    pos += vel * timestep;

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


