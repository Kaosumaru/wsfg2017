#include "GameInitializer.h"

#include "Game/Model/Gem.h"
#include "Game/Model/Player.h"
#include "Script/ScriptClassParser.h"


using namespace MX;

using namespace std;

void BH::GameInitializer::Init()
{
	Script::onParsed.static_connect([&]() { AfterScriptParse(); });
	AfterScriptParse();

}


void BH::GameInitializer::AfterScriptParse()
{
    auto &script = Script::current();
    script.SetPairFunctor("Gem.Exploding", []()-> float { return Context<Gem>::current()._exploding; });
    script.SetPairFunctor("Gem.ExplosionPercent", []()-> float 
    { 
        if (!Context<Gem>::isCurrent())
            return 0.0f;
        return Context<Gem>::current().explosionPercent(); 
    });

    script.SetPairFunctor("Player.Number", []()-> float
    {
        if (!Context<Player>::isCurrent())
            return 0.0f;
        return Context<Player>::current().number();
    });

    script.SetPairFunctor("Action.CanPay", []()-> float
    {
        if (!Context<Action>::isCurrent())
            return 0.0f;
        return Context<Action>::current().canPay() ? 1.0f : 0.0f;
    });
}
