#pragma once

#include "rendering/primitiverenderer.h"
#include "level.h"
#include "rendering/camera.h"
#include "player.h"

namespace jnr {

    struct DebugOptions {
        int timestep = 144;
        float speed = 1.0f;
        bool movement_smoothing = true;
        bool showPlayerHitbox = false;
    };

    class PlayMode : private NonCopyable{
    protected:
        std::shared_ptr<DebugOptions> debugOptions;
        std::shared_ptr<Camera> cam;
        std::shared_ptr<Player> player;
        std::shared_ptr<PrimitiveRenderer> primitiveRenderer;
        std::shared_ptr<LevelWrapper> level;
    private:
        Input lastInput;
    public:
        PlayMode();
        virtual void update(float timestep);
        virtual void render(float delta, float catchup);
        virtual void onGui();
        virtual bool canClose(){return true;}
        void run();
    };

}



