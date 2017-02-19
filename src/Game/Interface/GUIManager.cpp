#include "GUIManager.h"
#include "application/Application.h"
#include "Game/Resources/Paths.h"
#include "Game/Resources/Resources.h"
#include "Script/Script.h"
#include "Script/ScriptObject.h"
#include "Application/Window.h"
#include <iostream>

#include "Widgets/Widget.h"
#include "Widgets/Layouters/StackWidget.h"
#include "Widgets/Label.h"
#include "Widgets/Controllers/ListController.h"
#include "Widgets/Layouters/SimpleLayouters.h"
#include "Widgets/Layouters/ScriptLayouters.h"

#include "Devices/Keyboard.h"

#include "Game/GameInitializer.h"
#include "Game/Main/MainGame.h"
#include "Widgets/Animations/Animations.h"
#include "Sound/Stream.h"

using namespace MX;
using namespace BH;
using namespace std;


class MCreditsScene : public MX::FullscreenDisplayScene, public MX::SignalTrackable
{
	std::shared_ptr<MX::Widgets::ScriptLayouterWidget> _bgLayouter;
public:
	MCreditsScene()
	{
		{
			auto bg = std::make_shared<MX::Widgets::ScriptLayouterWidget>();
			bg->AddStrategy(std::make_shared<MX::Strategies::FillInParent>());
			bg->SetLayouter("GUI.Credits.Layouter");
			AddActor(bg);
			_bgLayouter = bg;
		}

		auto button = std::make_shared<MX::Widgets::LabelButton>(L"");
		_bgLayouter->AddNamedWidget("Button.Exit", button);
		button->onClicked.connect([&]() { SpriteSceneStackManager::manager_of(this)->PopScene(std::make_shared<MoveBitmapTransition>(false)); }, this);
	}
};


class MMenuScene : public MX::FullscreenDisplayScene, public MX::SignalTrackable
{
	std::shared_ptr<MX::Widgets::ScriptLayouterWidget> _bgLayouter;
public:
	MMenuScene()
    {
        {
            auto bg = std::make_shared<MX::Widgets::ScriptLayouterWidget>();
            bg->AddStrategy(std::make_shared<MX::Strategies::FillInParent>());
            bg->SetLayouter("GUI.MainMenu.Layouter");
            AddActor(bg);
            _bgLayouter = bg;
        }

        auto addButton = [&](const std::string& name)
        {
            auto wstr_name = loc(name);
            auto button = std::make_shared<MX::Widgets::LabelButton>(wstr_name);
            _bgLayouter->AddNamedWidget(name, button);
            return button;
        };

        addButton("Button.Game")->onClicked.connect([&]() { OnGame(2); }, this);
        addButton("Button.Credits")->onClicked.connect([&]() { OnCredits(); }, this);
    }

    void OnGame(int players, bool animate = true)
    {
        auto game = std::make_shared<MainGame>(players);

        if (animate)
            SpriteSceneStackManager::manager_of(this)->PushScene(game, std::make_shared<MoveBitmapTransition>(true));
        else
            SpriteSceneStackManager::manager_of(this)->PushScene(game, nullptr);
    }

	void OnCredits()
	{
		auto credits = std::make_shared<MCreditsScene>();
		SpriteSceneStackManager::manager_of(this)->PushScene(credits, std::make_shared<MoveBitmapTransition>(true));
	}
protected:

    void OnExit()
    {
        MX::App::current().Quit();
    }

};



GuiManager::GuiManager()
{
    auto menu = std::make_shared<MMenuScene>();
	PushScene(menu);

	GameInitializer::Init();
#ifndef MX_GAME_RELEASE
    menu->OnGame(2, false);
#endif
}









