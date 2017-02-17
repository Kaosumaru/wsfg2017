//
//  main.m
//  TestAllegro
//
//  Created by Redefine on 1/22/13.
//  Copyright (c) 2013 Redefine. All rights reserved.
//

#include "application/application.h"
#include "application/window.h"
#include "game/ScriptInitializer.h"
#include "game/resources/Paths.h"
#include "game/resources/Resources.h"
#include "graphic/images/Image.h"
#include "graphic/renderer/MVP.h"
#include "graphic/opengl/Utils.h"
#include "devices/Keyboard.h"
#include "utils/ListFiles.h"
#include <iostream>



#if 0
#include "Game/TestManager.h"
using CurrentManager = BH::TestManager;
#else
#include "Game/Interface/GUIManager.h"
using CurrentManager = BH::GuiManager;
#endif

unsigned _width = 1280, _height = 800;
bool _fullscreen = false;

using namespace std;
using namespace MX;
class GameApp : public MX::App
{
public:
	GameApp()
	{

	}

protected:
      

	void SetResPath()
	{
#ifdef __EMSCRIPTEN__
		auto res_path = "/res/";
#else
#ifndef MX_GAME_RELEASE
		auto res_path = MX_DEBUG_RES_PATH;
#else
#if MX_PLATFORM_XCODE
		auto res_path = MX_DEBUG_RES_PATH;
#else
		auto res_path = "res/";
#endif
#endif

#ifdef MX_GAME_PERF_CHEATS
//		res_path = MX_DEBUG_RES_PATH;
#endif
#endif

		MX::Paths::get().SetResourcePath(res_path);
		MX::Paths::get().SetImagePath(res_path);
		MX::Paths::get().SetSoundPath(res_path);
		MX::Paths::get().SetStreamPath(res_path);
	}

	void OnPrepare() override
	{
		SetResPath();
		OpenMainWindow(1680, 1024, false);

		MX::Window::current().keyboard()->on_specific_key_down[SDL_SCANCODE_ESCAPE].static_connect([&]() { Quit(); });
		MX::ScriptInitializer::ReloadScripts();
	}

	 

	void OnLoop()
	{
        CurrentManager::get().Run();
	}

	void OnRender()
	{
		MX::gl::Clear({ 0.0f, 0.0f, 0.0f, 1.0f });
        
        CurrentManager::get().Draw();
	}

public:


};


int main(int argc, char* argv[])
{
	GameApp app;
	app.Run();
	return 0;
}
