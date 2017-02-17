#include "game/resources/Paths.h"
#include "Game/resources/Resources.h"
#include "script/Script.h"
#include "script/ScriptObject.h"
#include "application/Window.h"
#include "script/class/ScriptSoundClass.h"
#include "Game.h"

using namespace BH;


Game::Game(int players)
{
    for (int i = 0; i < players; i++)
    {
        auto player = std::make_shared<Player>(players == 1 ? 3 : i);
        _players.push_back(player);
    }

#ifndef MX_GAME_RELEASE
    if (players == 1)
    {
        auto &player = _players.front();
        for (auto &m : player->stats().manas())
        {
            m->_max = 10000.0f;
            m->Gain(10000.0f);
        }
            
    }
#endif
}

void Game::Update()
{
    auto g1 = Context<Game>::Lock(this);
    if (_end)
        return;
    for (auto& player : _players)
        player->Update();

    bool lostFirst = false;
    bool lostSecond = false;

    if (_players.size() > 0)
        lostFirst = _players[0]->_lost;

    if (_players.size() > 1)
        lostSecond = _players[1]->_lost;

    if (!lostFirst && !lostSecond)
        return;

    _end = true;

    int result = 0;
    if (lostFirst)
        result = 1;
    if (lostSecond)
        result = 0;
    if (lostSecond && lostFirst)
        result = -1;
    onGameWon(result);

}