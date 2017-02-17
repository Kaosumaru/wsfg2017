#ifndef BHDEBUGCHEATS
#define BHDEBUGCHEATS
#include<memory>
#include "Scene/Sprites/SpriteScene.h"
#include "Scene/Managers/SceneManager.h"
#include "Utils/Utils.h"

namespace BH
{
    struct CheatObject
    {
        virtual ~CheatObject() {}
    };

    std::shared_ptr<CheatObject> CreateCheats();
    
}


#endif
