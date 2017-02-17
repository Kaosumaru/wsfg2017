#include "MainGame.h"
#include "Game/Resources/Paths.h"
#include "Game/Resources/Resources.h"
#include "Script/Script.h"
#include "Script/ScriptObject.h"
#include "Application/Window.h"


#include "Devices/Mouse.h"
#include "Devices/Keyboard.h"
#include "Sound/Stream.h"

#include "Script/ScriptClassParser.h"

#include "Game/DebugCheats.h"

#include "Widgets/Widget.h"
#include "Widgets/Layouters/StackWidget.h"
#include "Widgets/Label.h"
#include "Widgets/Controllers/ListController.h"
#include "Widgets/Layouters/SimpleLayouters.h"
#include "Widgets/Layouters/ScriptLayouters.h"

#include "Game/View/View.h"
#include "SDL_keycode.h"

using namespace MX;
using namespace BH;
using namespace std;

MainGame::MainGame(int players) : DisplaySceneTimer(MX::Window::current().size())
{
    _game = std::make_shared<Game>(players);
    _cheats = CreateCheats();
    Context<BaseGraphicScene>::SetCurrent(*this);
    
    auto bg = std::make_shared<MX::Widgets::ScriptLayouterWidget>();
    _bg = bg;
    {
        bg->AddStrategy(std::make_shared<MX::Strategies::FillInParent>());
        bg->SetLayouter("Game.Layouter");
        AddActor(bg);
    }

    if (players > 0)
    {
        auto player = _game->players()[0];
        auto levelView = std::make_shared<LevelView>(player, player->level());
        bg->AddNamedWidget("Player1.Level", levelView);

        auto actionsView = std::make_shared<ActionsView>(player);
        bg->AddNamedWidget("Player1.Actions", actionsView);
    }

    if (players > 1)
    {
        auto player = _game->players()[1];
        auto levelView = std::make_shared<LevelView>(player, player->level());
        bg->AddNamedWidget("Player2.Level", levelView);

        auto actionsView = std::make_shared<ActionsView>(player);
        bg->AddNamedWidget("Player2.Actions", actionsView);
    }

    _game->onGameWon.connect([&](int r) { onGameWon(r); }, this);
    MX::Window::current().keyboard()->on_specific_key_down[SDL_SCANCODE_ESCAPE].connect(std::bind(&MainGame::onExit, this), this);


    PlayGameMusic();
}

void MainGame::PlayGameMusic()
{
    auto musicPath = "audio/music/InfiniteDescent.ogg";
    MX::Sound::StreamManager::get().SetCurrent(Resources::get().loadStream(musicPath));
}

void MainGame::Run()
{
    if (_game)
        _game->Update();
    MX::DisplaySceneTimer::Run();
}

void MainGame::onGameWon(int r)
{
    std::string path;
    if (r == 0)
        path = "Game.Victory.Screen.Player1.Drawer";
    else if (r == 1)
        path = "Game.Victory.Screen.Player2.Drawer";
    else
        path = "Game.Victory.Screen.Tie.Drawer";

    auto widget = std::make_shared<MX::Widgets::Widget>();
    auto slotSetDrawer = MX::Widgets::DrawerManager::drawer(path);
    widget->SetDrawer(slotSetDrawer);
    _bg->AddNamedWidget("Victory", widget);
}


void MainGame::onExit()
{
    MX::Sound::StreamManager::get().SetCurrent(nullptr);
    SpriteSceneStackManager::manager_of(this)->PopScene(std::make_shared<MoveBitmapTransition>(false));
}

