#include "Events.h"

#include "Game.h"
#include "Player.h"
#include "widgets/Drawers/Drawer.h"
#include "script/ScriptClassParser.h"
#include "Scene/Script/Event.h"

using namespace BH;

class CommonEvent : public MX::Event
{
public:
	CommonEvent(MX::LScriptObject& script)
	{

	}

	Player& player()
	{
		return Context<Player>::current();
	}

	Player& enemyPlayer()
	{
		auto &game = Context<Game>::current();

		auto &current = Context<Player>::current();

		for (auto &player : game.players())
		{
			if (&current != player.get())
				return *player;
		}
		return current;
	}

	Level &enemyLevel()
	{
		return *(enemyPlayer().level());
	}

	Level &level()
	{
		return *(player().level());
	}
};

class DamageEnemy : public CommonEvent
{
public:
	DamageEnemy(MX::LScriptObject& script) : CommonEvent(script)
	{
		script.load_property_child( _damage, "Damage" );
	}

	void Do() override
	{
		enemyPlayer().stats().hp()->Lose(_damage);
	}
protected:
	float _damage = 5.0f;
};
MXREGISTER_CLASS(L"Event.DamageEnemy", DamageEnemy)