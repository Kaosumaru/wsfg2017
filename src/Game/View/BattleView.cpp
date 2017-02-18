#include "BattleView.h"
#include "Game/Model/Rule.h"
#include "Scene/Sprites/ScriptableSpriteActor.h"

using namespace MX;
using namespace BH;

class BattleView : public MX::Widgets::ScriptLayouterWidget
{
public:
	BattleView(const Player::pointer& player1, const Player::pointer& player2)
	{
		SetLayouter("Game.Battle.View.Layouter");
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
