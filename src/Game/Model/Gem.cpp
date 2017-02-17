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