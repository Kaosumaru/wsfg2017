#include "Player.h"

#include "Game.h"
#include "SDL_keycode.h"
using namespace BH;



void PlayerControlSchema::SetupForPlayer(int number)
{
    using namespace MX::Game;
    direction.SetFirstTickMultiplier(1.3f);

	{
		int joy = number == 1 ? 1 : 0;
		direction.BindJoypad(joy);

		useSkill.bindJoyButton(0, joy, 0);
		useSkill.bindJoyButton(1, joy, 1);
		useSkill.bindJoyButton(2, joy, 2);
	}

    if (number == 0)
    {
        direction.bindKeysWSAD();
		
		useSkill.bindKey(0, SDL_SCANCODE_V);
		useSkill.bindKey(1, SDL_SCANCODE_B);
		useSkill.bindKey(2, SDL_SCANCODE_N);
    }
    else if (number == 1)
    {
		direction.bindKeysArrows();

        useSkill.bindKey(0, SDL_SCANCODE_COMMA);
        useSkill.bindKey(1, SDL_SCANCODE_PERIOD);
        useSkill.bindKey(2, SDL_SCANCODE_SLASH);
    }
    else
    {
        direction.bindKeysArrows();
		direction.BindJoypad(0);

        useSkill.bindKey(0, SDL_SCANCODE_Z);
        useSkill.bindKey(1, SDL_SCANCODE_X);
        useSkill.bindKey(2, SDL_SCANCODE_C);
        useSkill.bindKey(2, SDL_SCANCODE_N);
    }
}

Player::Player(int number)
{
    _number = number;
    auto g1 = Context<Player>::Lock(this);
    auto g12 = Context<Level>::Lock(_level);

    _controlSchema.SetupForPlayer(number);
    _controlSchema.useSkill.whileOn([&](unsigned index) 
    {
        return [&, index]() { actions().UseAction(index); };
    });

    _controlSchema.direction.onTick.static_connect([&](auto &v) 
    {
		bool canMove = false;
		if (_actions.list().empty() || !_actions.list()[0]->blocksMovement())
			canMove = true;

        if (!_controlSchema.useSkill.actions[0].state)
			canMove = true;

		if (canMove)
		{
			auto oldPosition = _level->selector()->pos();
			_level->selector()->Move(v);
			auto newPosition = _level->selector()->pos();

			if (isHoldingGem() && oldPosition != newPosition)
			{
				_level->SwapGems(oldPosition, newPosition);
			}
		}
			
    });



	{
		MX::ScriptObjectString script("Game.Player");

#if 0
		script.load_property(_horizSelector, "HorizSelector");
		if (!_horizSelector)
		{
			_actions.Add(ActionCreator::createSwap());
		}
		else
		{
			_actions.Add(ActionCreator::createHorizSwap());
		}
#endif

		std::vector<Action::pointer> actions;
		script.load_property(actions, "Actions");

		for (auto &action : actions)
			_actions.Add(action);

		float health = 25.0f;
		script.load_property(health, "Health");
		stats().hp()->Reset(health, health);
	}


#if 0
    if (number < 3)
    {
        _actions.Add(ActionCreator::createFrostbolt());
        _actions.Add(ActionCreator::createGemRain());
    }
    else
    {
#ifdef _DEBUG
        //_actions.Add(ActionCreator::createGemRain());
#endif
    }
#endif 


    //SwapGems
    //Fireball
    //Freezeball
    //Confuse
    //SlowTime - passive
}

bool Player::HoldGem(const Gem::pointer& gem)
{
	bool willHold = gem != nullptr;

	if (isHoldingGem() == willHold)
	{
		assert(false);
		return false;
	}

	if (gem)
	{
		if (gem->_held)
			return false;
		if (gem->_frozen)
			return false;
		if (gem->_falling)
			return false;
		if (gem->_wantToExplode)
			return false;
	}
		

	if (_heldGem)
		_heldGem->_held = false;
	_heldGem = gem;
	if (_heldGem)
		_heldGem->_held = true;

	//TODO ifdestroyed, stop holding
}

void Player::Update()
{
    if (_lost)
        return;

	auto guard2 = Context<MX::FunctorsQueue>::Lock(_queue);
	auto guard = Context<MX::Time::Timer>::Lock(_timer);
    auto g1 = Context<Player>::Lock(this);
    auto g12 = Context<Level>::Lock(_level);
	_queue.Run();
    _actions.Update();

	if (_heldGem && _heldGem->_wantToExplode)
		HoldGem(nullptr);

    _controlSchema.Run();
    /*
    if (!_controller->wantsToUseAction())
        _level->selector()->Move(_controller->wantsDirection());
    _controller->Update();
    */
    _level->Update();
	_timer.Step();
}