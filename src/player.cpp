#include <iostream>
#include "player.h"
#include "mixed.h"
#include <limits>
#include <utility>
#include <imgui.h>

using namespace jnr;
using namespace glm;
using namespace playerstates;

const float jump_impulse = 700;

const float gravity = -1200;
const float fall_factor = 2.0f;
const float short_jump_factor = 3.0f;
const float max_fall_speed = -1300;

const float max_speed = 400;
const float wall_slide_friction = 12;
const float ground_friction = 9;
const float ground_acceleration = 1600;
const float ground_reactivity_factor = 3;
const float air_friction = 1;
const float air_acceleration = 2000;
const float air_reactivity_factor = 0.8f;

const float jump_buffer_time = 0.08f;
const float ground_coyote_time = 0.07f;
const float wall_coyote_time = 0.1f;

namespace jnr::playerstates::states {
    State jumping    {0, "Jumping",StateTraits::IN_AIR};
    State walking    {1, "Running",StateTraits::CAN_JUMP};
    State still      {2, "Still",StateTraits::CAN_JUMP};
    State falling    {3, "Falling",(StateTraits::IN_AIR | StateTraits::FALLING)};
    State wall_slide {4, "Sliding",StateTraits::NONE};
    State idle       {5, "Idle",StateTraits::CAN_JUMP};
}

Player::Player(float x, float y, const std::string& creature_path, const std::string& texture_path) :
    hitbox{vec2(-20,0),vec2(20,70)},
    foot_hitbox{vec2(-19, -1), vec2(19,1)},
    l_arm_hitbox{vec2(-23,15),vec2(-19,69)},
    r_arm_hitbox{vec2(19,15),vec2(23,69)},
    pos(x,y),
    vel(0,0),
    force(0,0),
    state(&states::jumping){


    CreatureModule::CreatureLoadDataPacket json_data;
    CreatureModule::LoadCreatureJSONData(creature_path, json_data);
    auto creature =  std::make_shared<CreatureModule::Creature>(json_data);
    creature_renderer = std::make_unique<CreatureRenderer>(creature, std::make_shared<Texture>(texture_path));
    creature_manager = std::make_unique<CreatureModule::CreatureManager>(creature);

    creature_manager->CreateAnimation(json_data, "Running");
    creature_manager->CreateAnimation(json_data, "Idle");
    creature_manager->CreateAnimation(json_data, "Still");
    creature_manager->CreateAnimation(json_data, "Jumping");
    creature_manager->CreateAnimation(json_data, "Falling");
    creature_manager->CreateAnimation(json_data, "Sliding");
    creature_manager->SetActiveAnimationName("Still");
    creature_manager->SetAutoBlending(true);
    creature_manager->SetIsPlaying(true);

}


void Player::update(float timestep, Input input) {
    if(!level)
        return;

    statetime += timestep;
    if(*state == states::still && statetime > 2)
        setState(&states::idle);


    force = vec2(0, gravity);
    if(all(state->traits, StateTraits::FALLING))
        force *= fall_factor;
    else if (short_jump){
        force *= short_jump_factor;
    }

    if(!input.jump && vel.y > 0)
        short_jump = true;

    remainingJumpTime -= timestep * (input.jump ? 1.0f : 3.0f);
    if(input.jumpDown)
        remainingJumpTime = jump_buffer_time;

    remainingGroundCoyoteTime -= timestep;
    if(all(state->traits, StateTraits::CAN_JUMP))
        remainingGroundCoyoteTime = ground_coyote_time;

    remainingLeftWallCoyoteTime -= timestep;
    if(onLeftWall)
        remainingLeftWallCoyoteTime = wall_coyote_time;

    remainingRightWallCoyoteTime -= timestep;
    if(onRightWall)
        remainingRightWallCoyoteTime = wall_coyote_time;

    if (remainingJumpTime > 0) {
        if(remainingLeftWallCoyoteTime > 0 || remainingGroundCoyoteTime > 0 || remainingRightWallCoyoteTime > 0){
            float lwc = wall_coyote_time - remainingLeftWallCoyoteTime;
            float gwc = ground_coyote_time - remainingGroundCoyoteTime;
            float rwc = wall_coyote_time - remainingRightWallCoyoteTime;
            if(gwc <= lwc && gwc <= rwc)
                vel.y = jump_impulse;
            else
                vel = vec2(((lookToLeft = (lwc > rwc)) ? -1 : 1) * 0.7, 1) * jump_impulse;
            force.y = 0;
            setState(&states::jumping);
            remainingJumpTime = 0;
            remainingGroundCoyoteTime = 0;
            remainingLeftWallCoyoteTime = 0;
            remainingRightWallCoyoteTime = 0;
            short_jump = false;
        }
    }

    float base = (all(state->traits, StateTraits::IN_AIR) ? air_acceleration : ground_acceleration) * input.move.x;
    if(input.move.x * vel.x < 0.0)
        base *= (all(state->traits, StateTraits::IN_AIR) ? air_reactivity_factor : ground_reactivity_factor);
    force.x += base;

    if(force.x != 0)
        lookToLeft = force.x < 0;

    vel.y += force.y * timestep;
    if(vel.y < max_fall_speed)
        vel.y = max_fall_speed;

    if(abs(vel.x + force.x * timestep) <= fmax(max_speed, abs(vel.x)))
        vel.x += force.x * timestep;
    else if(abs(vel.x) < max_speed)
        vel.x = clamp(vel.x + force.x * timestep, -max_speed, max_speed);

    float remainingtime = timestep;
    //std::cout << "start (" << to_string(vel) << ")";
    for(int i = 0; i < 10 && remainingtime > timestep * 0.05f; i++) {
        CollisionInfo info = jnr::checkSweptAABB(pos, vel * remainingtime, hitbox, level->getHitboxes());
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
    if(jnr::checkAABB(pos, foot_hitbox, level->getHitboxes())) {
        if(abs(force.x) > 10){
            setState(&states::walking);
        } else if(!(*state == states::idle)){
            setState(&states::still);
        }
    }else if(vel.y < 0 && !all(state->traits, StateTraits::FALLING))
        setState(&states::falling);

    onLeftWall = jnr::checkAABB(pos, l_arm_hitbox, level->getHitboxes());
    onRightWall = jnr::checkAABB(pos, r_arm_hitbox, level->getHitboxes());

    if(((onLeftWall && force.x < 0) || (onRightWall && force.x > 0)) && all(state->traits, StateTraits::IN_AIR)) // && all(state->traits, StateTraits::FALLING)
        setState(&states::wall_slide);

    if(*state == states::wall_slide && !((onLeftWall && force.x < 0) || (onRightWall && force.x > 0)))
        setState(vel.y < 0 ? &states::falling : &states::jumping);

    //if(input.jump && all(state->traits, StateTraits::FALLING))
    //    vel.y -= fabsmin(vel.y * 18 * timestep, vel.y);

    if(*state == states::wall_slide && vel.y < 0)
        vel.y -= fabsmin(vel.y * wall_slide_friction * timestep, vel.y);
    if(abs(force.x) < 10)
        vel.x -= fabsmin(vel.x * (all(state->traits, StateTraits::IN_AIR) ? air_friction : ground_friction) * timestep, vel.x);
    if(abs(vel.x) > max_speed)
        vel.x -= fabsmin(faddabs(vel.x, -max_speed) * (all(state->traits, StateTraits::IN_AIR) ? air_friction : ground_friction) * timestep, faddabs(vel.x, -max_speed));
    if(abs(vel.x) < 1)
        vel.x = 0;

}

void Player::setState(playerstates::State *s) {
    if(*s == *state)
        return;
    statetime = 0;
    state = s;
}

void Player::draw(float delta, float catchup, Camera& cam) {
    creature_manager->AutoBlendTo(state->name, delta * 10);
    creature_manager->SetMirrorY(lookToLeft);
    creature_manager->Update(delta);
    creature_manager->GetCreature()->FillRenderColours(255,255,255,255);
    creature_renderer->draw(pos + vel * catchup, 17, cam);
}

void Player::ongui(GuiSection s) {
    if(s == INFO) {
        ImGui::Text("vel: [%.2f,%.2f]", vel.x, vel.y);
        ImGui::Text("pos: [%.2f,%.2f]", pos.x, pos.y);
        ImGui::Text("state: %s", state->name.c_str());
    }
}

void drawAABB(PrimitiveRenderer& pr, vec2 pos, AABB& aabb, colors::color c){
    pr.drawAABB(pos + aabb.low, pos + aabb.high, c, 0.3f);
}

void Player::drawDebug(float delta, float catchup, PrimitiveRenderer& pr) {
    drawAABB(pr, pos + vel * catchup, hitbox      , colors::colorF(0.6f, 1.00f,0.42f,0.42f));
    drawAABB(pr, pos + vel * catchup, r_arm_hitbox, colors::colorF(0.6f, 0.42f,0.42f,1.00f));
    drawAABB(pr, pos + vel * catchup, l_arm_hitbox, colors::colorF(0.6f, 0.42f,0.42f,1.00f));
    drawAABB(pr, pos + vel * catchup, foot_hitbox , colors::colorF(0.6f, 0.42f,1.00f,0.42f));

}

void Player::setLevel(std::shared_ptr<Level> l) {
    level = std::move(l);
}

const Level *Player::getLevel() const {
    return level.get();
}

