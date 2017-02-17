#ifndef BHVIEW
#define BHVIEW
#include<memory>
#include "Scene/Sprites/SpriteScene.h"
#include "Scene/Managers/SceneStackManager.h"
#include "Utils/Utils.h"
#include "Game/Model/Level.h"
#include "Game/Model/PLayer.h"
#include "Widgets/Layouters/ScriptLayouters.h"

namespace BH
{
    class ActionsView : public MX::Widgets::ScriptLayouterWidget, public MX::SignalTrackable
    {
    public:
        ActionsView(const Player::pointer& player);
        Player::pointer _player;
    };

    
    class LevelView : public MX::Widgets::ScriptLayouterWidget, public MX::SignalTrackable
    {
    public:
        LevelView(const Player::pointer& player, const Level::pointer& level);

        void Run() override;
        void Draw(float x, float y) override;
    protected:
        void onNewLevel();

        Level::pointer _level;
        Player::pointer _player;
    };



    class SelectorView : public MX::Widgets::ScriptLayouterWidget, public MX::SignalTrackable
    {
    public:
        SelectorView(const Selector::pointer& selector);
        glm::vec2 positionFromSelector();
    protected:
        Selector::pointer _selector;
    };
    
}


#endif
