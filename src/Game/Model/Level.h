#ifndef BHLEVEL
#define BHLEVEL
#include<memory>
#include "utils/Utils.h"
#include "Gem.h"
#include "utils/SignalizingVariable.h"
#include "scene/script/Event.h"

namespace BH
{
    class ActionList;
    class Selector
    {
    public:
        using pointer = std::shared_ptr<Selector>;

        enum class Direction
        {
            None,
            Up,
            Down,
            Left,
            Right
        };

        static auto deltaFromDirection(Direction direction)
        {
            static std::vector<glm::ivec2> deltas =
            {
                { 0 ,  0 },
                { 0 , -1 },
                { 0 ,  1 },
                { -1,  0 },
                { 1 ,  0 }
            };
            return deltas[(int)direction];
        }

        auto pos() { return (glm::ivec2) position.directValueAccess(); }
        SignalizingVariable<glm::ivec2> position = glm::ivec2{ 0,6 };

        bool Move(const glm::ivec2& direction);
        void ForceMove(Direction direction);

    };



    class Level
    {
    public:
        using pointer = std::shared_ptr<Level>;

        Level();

        bool InsertGem(const glm::ivec2& pos, const Gem::pointer& gem);
        bool SwapGems(const glm::ivec2& pos1, const glm::ivec2& pos2);
        void DestroyGem(const glm::ivec2& pos);

        void RandomizeAll();
        void RandomizeHalf();

        void Update();

        auto& at(const glm::ivec2& pos)
        {
            static Gem::pointer gem;
            if (!containsPosition(pos))
                return gem;
            return _gems[pos.x + pos.y * _width];
        }

        auto& gems() { return _gems; }
        auto& selector() { return _selector; }

        auto& width() { return _width; }
        auto& height() { return _height; }

        bool containsPosition(const glm::ivec2& pos)
        {
            if (pos.x < 0 || pos.y < 0)
                return false;
            if (pos.x >= width() || pos.y >= height())
                return false;
            return true;
        }

        auto& movementRule()
        {
            return _movementRule;
        }

        int nonExplodingColorAt(glm::ivec2 pos);
        void InsertRandomNonExplodingGem(glm::ivec2 pos);

        MX::Signal<void(const Gem::pointer&)> onCreatedGem;

        float startTime() { return _startTime; }

        MX::Signal<void(glm::ivec2 pos, MX::EventHolder &e)> onEvent;
    protected:
        float _startTime = -1;

        int _width = 6;
        int _height = 13;
        std::vector<Gem::pointer> _gems;
        Selector::pointer _selector = std::make_shared<Selector>();
        std::shared_ptr<ActionList> _rules;
        std::shared_ptr<class LevelMovement> _movementRule;
    };
}


#endif
