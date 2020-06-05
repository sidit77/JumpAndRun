#include "keyhelper.h"

namespace jnr::KeyHelper {

    bool isKeyDown(Key key){
        return ImGui::IsKeyDown((uint32_t)(key));
    }

    bool isKeyPressed(Key key){
        return ImGui::IsKeyPressed((uint32_t)(key), false);
    }

    bool isKeyReleased(Key key){
        return ImGui::IsKeyReleased((uint32_t)(key));
    }

    MKey getModifierKeys(){
        MKey result = MKey::NONE;
        if(isKeyDown(Key::LEFT_SHIFT) || isKeyDown(Key::RIGHT_SHIFT))
            result = result | MKey::SHIFT;
        if(isKeyDown(Key::LEFT_CONTROL) || isKeyDown(Key::RIGHT_CONTROL))
            result = result | MKey::CONTROL;
        if(isKeyDown(Key::LEFT_ALT) || isKeyDown(Key::RIGHT_ALT))
            result = result | MKey::ALT;
        if(isKeyDown(Key::LEFT_SUPER) || isKeyDown(Key::RIGHT_SUPER))
            result = result | MKey::SUPER;
        return result;
    }

    bool areMKeysDown(MKey key){
        return (((uint32_t)key & 0xffff0000u) & (uint32_t)getModifierKeys()) == ((uint32_t)key & 0xffff0000u);
    }

    bool isKeyDown(MKey key){
        return areMKeysDown((MKey)((uint32_t)key)) && ImGui::IsKeyDown((int)((uint32_t)(key) & 0x0000ffffu));
    }

    bool isKeyPressed(MKey key){
        return areMKeysDown((MKey)((uint32_t)key)) && ImGui::IsKeyPressed((int)((uint32_t)(key) & 0x0000ffffu, false));
    }

    bool isKeyReleased(MKey key){
        return areMKeysDown((MKey)((uint32_t)key)) && ImGui::IsKeyReleased((int)((uint32_t)(key) & 0x0000ffffu));
    }

}