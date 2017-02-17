#ifndef BHPLAYER
#define BHPLAYER
#include<memory>
#include "scene/sprites/SpriteScene.h"
#include "scene/managers/SceneManager.h"
#include "utils/Utils.h"
#include "Action.h"
#include "Level.h"
#include "Mana.h"
#include "game/controlscheme/ControlScheme.h"

namespace BH
{

    class PlayerControlSchema : public MX::Game::ControlScheme
    {
    public:
        using MX::Game::ControlScheme::ControlScheme;

        void SetupForPlayer(int number);

        MX::Game::TickingTargetDirection<glm::ivec2> direction{ this, 0.1f };
        MX::Game::ActionList<MX::Game::Action, 4> useSkill{ this };
    };

    class Player
    {
    public:
        using pointer = std::shared_ptr<Player>;

        Player(int number);

        auto &actions() { return _actions; }
        auto &level() { return _level; }
        auto &schema() { return _controlSchema; }
        auto &stats() { return _stats; }

        auto selectorPosition()
        {
            return _level->selector()->pos();
        }

        void Update();

        SignalizingVariable<bool> _lost = false;

        int number() { return _number; }
    protected:
        int                 _number = 0;
        Stats               _stats;
        
        Level::pointer _level = std::make_shared<Level>();
        ActionList _actions;
        PlayerControlSchema _controlSchema;
    };
}


#endif
