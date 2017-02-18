#include "Gem.h"
#include "game/resources/Paths.h"
#include "game/resources/Resources.h"
#include "Script/Script.h"
#include "Game.h"

using namespace BH;

Gem::Gem(int type)
{
    _type = type;
}


Gem::pointer Gem::random()
{
    int randomType = Random::randomInt(0, maxType-1);
    return std::make_shared<Gem>(randomType);
}

void Gem::Freeze()
{
	if (!_frozen)
	{
		auto &queue = MX::FunctorsQueue::current();
		queue.planWeakFunctor(7.0f, [&]() { _frozen = false; }, shared_from_this());
	}
		
	_frozen = true;
}