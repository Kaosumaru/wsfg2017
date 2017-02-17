#ifndef BHMGUIMANAGER
#define BHMGUIMANAGER
#include<memory>
#include "Scene/Sprites/SpriteScene.h"
#include "Scene/Managers/SceneStackManager.h"
#include "Utils/Utils.h"

namespace BH
{
    
class GuiManager : public MX::SpriteSceneStackManager, public DeinitSingleton<GuiManager>
{
public:
	GuiManager();
};
    
}


#endif
