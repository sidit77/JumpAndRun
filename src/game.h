#pragma once

#include <vector>
#include "rendering/primitiverenderer.h"
#include <GLFW/glfw3.h>
#include "level.h"
#include "rendering/camera.h"
#include "service.h"
#include "player.h"

namespace jnr {

    struct DebugOptions {
        int timestep = 144;
        float speed = 1.0f;
        bool movement_smoothing = true;
        bool showPlayerHitbox = false;
    };

    class GameMode : private NonCopyable {
    protected:
        std::shared_ptr<DebugOptions> debugOptions;
        std::shared_ptr<Camera> cam;
        std::shared_ptr<Player> player;
        std::shared_ptr<PrimitiveRenderer> primitiveRenderer;
        std::shared_ptr<LevelWrapper> level;
    public:
        explicit GameMode(const GameMode* parent){
            debugOptions = parent->debugOptions;
            cam = parent->cam;
            player = parent->player;
            primitiveRenderer = parent->primitiveRenderer;
            level = parent->level;
        };
        GameMode() :
        debugOptions(std::make_shared<DebugOptions>()),
        cam(std::make_shared<Camera>()),
        player(std::make_shared<Player>("assets/character/character_data.json", "assets/character/character_atlas.png")),
        primitiveRenderer(std::make_shared<PrimitiveRenderer>()),
        level(std::make_shared<LevelWrapper>(getOrDefault<std::string>((*jnr::services::config)["level"]["name"], "assets/levels/level1.dat")))
        {};

        virtual bool canClose(){return true;}
        virtual void update(float timestep) = 0;
        virtual void render(float delta, float catchup) = 0;
        virtual void onGui() = 0;
        DebugOptions& getDebugOptions(){return *debugOptions;}
    };

    class PlayMode : public GameMode {
    private:
        Input lastInput;
    public:
        PlayMode() : GameMode(){};
        PlayMode(const GameMode* parent) : GameMode(parent){};
        void update(float timestep) override;
        void render(float delta, float catchup) override;
        void onGui() override;
    };

}



