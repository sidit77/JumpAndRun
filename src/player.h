#pragma once

#include <glm.hpp>
#include <utility>
#include <vector>
#include "physics.h"
#include "mixed.h"

namespace jnr {

    namespace playerstates {
        enum class StateTraits : glm::uint32 {
            NONE = 0,
            CAN_JUMP = (1U << 0U),
            IN_AIR = (1U << 1U),
            FALLING = (1U << 2U),
        };


        constexpr enum StateTraits operator|(const enum StateTraits selfValue, const enum StateTraits inValue) {
            return static_cast<StateTraits>(static_cast<std::underlying_type<StateTraits>::type>(selfValue) |
                                            static_cast<std::underlying_type<StateTraits>::type>(inValue));
        }

        constexpr bool any(const enum StateTraits selfValue, const enum StateTraits inValue) {
            return (static_cast<std::underlying_type<StateTraits>::type>(selfValue) &
                    static_cast<std::underlying_type<StateTraits>::type>(inValue)) != 0;
        }

        constexpr bool all(const enum StateTraits selfValue, const enum StateTraits inValue) {
            return (static_cast<std::underlying_type<StateTraits>::type>(selfValue) &
                    static_cast<std::underlying_type<StateTraits>::type>(inValue)) == static_cast<std::underlying_type<StateTraits>::type>(inValue);
        }

        struct State : private NonCopyable{
            State(int i, std::string n, StateTraits t) : id(i), name(std::move(n)), traits(t) {};

            glm::uint32 id;
            std::string name;
            StateTraits traits;
        };

        constexpr bool operator==(const State& lhs, const State& rhs){
            return lhs.id == rhs.id;
        }
    }

    class Player {
    public:
        playerstates::State* state;

        glm::vec2 pos;
        glm::vec2 vel;
        glm::vec2 force;
        //bool inair;
        glm::uint8  onwall;
        //bool jumping{};
        AABB hitbox;
        AABB foot_hitbox;
        AABB l_arm_hitbox;
        AABB r_arm_hitbox;

        Player(float x, float y);

        void update(float timestep, Input input, const std::vector<jnr::AABB>&);

    };


}