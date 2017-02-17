#ifndef BHXGAME
#define BHXGAME
#include<memory>
#include "utils/Utils.h"
#include <memory> 
#include "Player.h"

namespace BH
{
    class Game
    {
    public:
        using pointer = std::shared_ptr<Game>;

        Game(int players = 2);

        auto &players() { return _players; }

        void Update();

        MX::Signal<void(int)> onGameWon;
    protected:
        bool _end = false;
        std::vector<std::shared_ptr<Player>> _players;
    };
}


#endif
