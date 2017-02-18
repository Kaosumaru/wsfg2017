#include "BattleView.h"
#include "Game/Model/Rule.h"
#include "Scene/Sprites/ScriptableSpriteActor.h"

using namespace MX;
using namespace BH;

class PlayerView : public MX::Widgets::ScriptLayouterWidget
{
public:
	PlayerView(const Player::pointer& player)
	{
		SetLayouter("Game.Player.View.Layouter");
	}

protected:
};


class BattleView : public MX::Widgets::ScriptLayouterWidget
{
public:
	BattleView(const Player::pointer& player1, const Player::pointer& player2)
	{
		SetLayouter("Game.Battle.View.Layouter");

		if (player1)
			AddNamedWidget("Player1", std::make_shared<PlayerView>(player1));
		if (player2)
			AddNamedWidget("Player2", std::make_shared<PlayerView>(player2));
	}

protected:
};


namespace BH
{
	std::shared_ptr<MX::Widgets::ScriptLayouterWidget> createBattleView(const Player::pointer& player1, const Player::pointer& player2)
	{
		return std::make_shared<BattleView>(player1, player2);
	}
}
