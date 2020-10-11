#pragma once

#include "rendering/primitiverenderer.h"
#include "level.h"
#include "rendering/camera.h"
#include "player.h"
#include "rendering/levelrenderer.h"

namespace jnr {

    struct DebugOptions {
        int timestep = 144;
        float speed = 1.0f;
        bool movement_smoothing = true;
        bool showPlayerHitbox = false;
    };

    class Game : private NonCopyable{
    protected:
        DebugOptions debugOptions;
        Camera cam;
        Player player;
        PrimitiveRenderer primitiveRenderer;
        LevelRenderer levelRenderer;
        LevelWrapper level;
    private:
        Input lastInput;
    public:
        Game();
        virtual ~Game() = default;
        virtual void update(float timestep);
        virtual void render(float delta, float catchup);
        virtual void onGui();
        virtual bool canClose(){return true;}
        void run();
    };

}



