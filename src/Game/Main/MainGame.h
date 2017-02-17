#ifndef BHMAINGAME
#define BHMAINGAME
#include<memory>
#include "Scene/Sprites/SpriteScene.h"
#include "Scene/Managers/SceneStackManager.h"
#include "Utils/Utils.h"
#include "Game/Model/Game.h"
#include "Widgets/Layouters/ScriptLayouters.h"

namespace BH
{
    
    class MainGame : public MX::DisplaySceneTimer, public MX::SignalTrackable
    {
    public:
        MainGame(int players);

        void Run() override;
    protected:
        void PlayGameMusic();
        void onGameWon(int r);
        void onExit();

        std::shared_ptr<MX::Widgets::ScriptLayouterWidget> _bg;
        std::shared_ptr<struct CheatObject> _cheats;
        Game::pointer _game;
    };
    
}


#endif
