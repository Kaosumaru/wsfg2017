#ifndef BHBATTLEVIEW
#define BHBATTLEVIEW
#include<memory>
#include "Scene/Sprites/SpriteScene.h"
#include "Scene/Managers/SceneStackManager.h"
#include "Utils/Utils.h"
#include "Game/Model/Level.h"
#include "Game/Model/PLayer.h"
#include "Widgets/Layouters/ScriptLayouters.h"

namespace BH
{
	std::shared_ptr<MX::Widgets::ScriptLayouterWidget> createBattleView(const Player::pointer& player1, const Player::pointer& player2);
}


#endif
