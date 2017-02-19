#include "BattleView.h"
#include "Game/Model/Rule.h"
#include "Scene/Sprites/ScriptableSpriteActor.h"

using namespace MX;
using namespace BH;

class PlayerView : public MX::Widgets::ScriptLayouterWidget
{
public:
	PlayerView(const Player::pointer& player, bool left)
	{
		_left = left;

		SetLayouter("Game.Player.View.Layouter");
		Script::onParsed.static_connect([&]() { onReload(); });
		onReload();
	}

	void onReload()
	{
		const std::string path = _left ? "Game.Char0.Animations" : "Game.Char1.Animations";
		ScriptObjectString script(path);
		script.load_property(_position, "Position");
		script.load_property(_scale, "Scale");

		loadAnimation(script, "Idle");

		loadAnimation(script, "Shield");
		loadAnimation(script, "Claws");
		loadAnimation(script, "Bear");
		loadAnimation(script, "Mind");
		loadAnimation(script, "Heal");
	}

	void loadAnimation(ScriptObjectString& script, const std::string& name)
	{
		std::shared_ptr<Graphic::SingleAnimation> animation;
		script.load_property(animation, name);
		auto anim = Graphic::Animation::Create(animation);
		_animations[name] = anim;

		if (!_idle)
		{
			_idle = anim;
			_current = anim;
		}
	}

	auto currentAnimation()
	{
		return _current;
	}

	void Run() override
	{
		if (currentAnimation())
			currentAnimation()->AdvanceTime(Time::Timer::current().elapsed_seconds());
	}

	void Draw(float x, float y) override
	{
		MX::Widgets::ScriptLayouterWidget::Draw(x, y);

		glm::vec2 p{ x,y };

		Graphic::Image::Settings::flipX = !_left;

		auto pos = _position + p;
		if (currentAnimation())
			currentAnimation()->DrawScaled({}, pos, { _scale, _scale });
		Graphic::Image::Settings::flipX = false;
	}

protected:
	float _scale = 1.0f;
	bool _left;
	std::map<std::string, std::shared_ptr<Graphic::Animation>> _animations;
	std::shared_ptr<Graphic::Animation> _idle;
	std::shared_ptr<Graphic::Animation> _current;
	glm::vec2 _position;
};


class BattleView : public MX::Widgets::ScriptLayouterWidget
{
public:
	BattleView(const Player::pointer& player1, const Player::pointer& player2)
	{
		SetLayouter("Game.Battle.View.Layouter");

		if (player1)
			AddNamedWidget("Player1", std::make_shared<PlayerView>(player1, true));
		if (player2)
			AddNamedWidget("Player2", std::make_shared<PlayerView>(player2, false));
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
