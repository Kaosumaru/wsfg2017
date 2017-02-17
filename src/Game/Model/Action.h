#ifndef BHACTION
#define BHACTION
#include<memory>
#include<vector>
#include "scene/sprites/SpriteScene.h"
#include "scene/managers/SceneManager.h"
#include "utils/Utils.h"
#include "Level.h"
#include "Mana.h"
#include "utils/Random.h"
#include "script/ScriptObject.h"
#include "scene/script/Event.h"

namespace MX
{
    namespace Widgets
    {
        class Drawer;
    }
};

namespace BH
{
    class Player;
    class Action : public MX::ScriptObjectString
    {
    public:
        Action(const std::string& objectName);

        void SetManaSource(const Mana::pointer& manaSource)
        {
            _manaSource = manaSource;
        }

        void SetManaSource(int i);

        using pointer = std::shared_ptr<Action>;

        Player& enemyPlayer();
        Level &enemyLevel();

        glm::ivec2 selectorPosition();
        bool levelContainsPosition(const glm::ivec2& pos);
        void destroyedGems(int color, int size);

        void reportPlayerLost();
        auto &level()
        {
            return Context<Level>::current();
        }

        float costProgress()
        {
            if (!_manaSource || _manaCost == 0.0f)
                return 1.0f;
            float p = _manaSource->_current / _manaCost;
            if (p > 1.0f)
                p = 1.0f;
            return p;
        }

        bool canPay()
        {
            if (!_manaSource || _manaCost == 0.0f)
                return true;
            return _manaSource->_current >= _manaCost;
        }

        glm::ivec2 wantsDelta();

        virtual bool Do();
        void Update()
        {
            if (_passive)
                Do();
        }

        auto& cooldownTimer() { return _cooldownTimer; }


        template<typename T>
        void enumerateBlocks3x3(const glm::ivec2& pos, T cb, bool enemy = false)
        {
            auto &l = enemy ? enemyLevel() : level();
            for (int x = -1; x < 2; x++)
                for (int y = -1; y < 2; y++)
                {
                    glm::ivec2 delta = { x, y };
                    auto p = pos + delta;

                    if (!levelContainsPosition(p))
                        continue;
                    auto gem = l.at(p);
                    cb(l, p, gem);
                }
        }

        template<typename T>
        void enumerateEnemyBlocks3x3(const glm::ivec2& pos, T cb)
        {
            return enumerateBlocks3x3(pos, cb, true);
        }

        glm::ivec2 randomPosition(int margin = 0)
        {
            glm::ivec2 r;
            r.x = Random::randomInt(margin, level().width() - 1 - margin);
            r.y = Random::randomInt(margin, level().height() - 1 - margin);
            return r;
        }

        auto& drawer() { return _drawer; }
        bool passive() { return _passive; }
    protected:
        virtual bool onDo()
        {
            return true;
        }

        bool _passive = false;
        glm::ivec2 _selectedGemPos = { -1,-1 };
        bool _selectedGemPosAtEnemy = false;

        Mana::pointer _manaSource;
        float _manaCost = 0.0f;
        float _cooldown = 0.0f;
        MX::Time::ManualStopWatchAbsolute   _cooldownTimer;
        MX::EventHolder   _doEvents;
        MX::EventHolder   _gemEvents;

        std::shared_ptr<MX::Widgets::Drawer> _drawer;
    };

    class SwapGemsAction : public Action
    {
    public:
        SwapGemsAction(const std::string& objectName) : Action(objectName) {}

        bool Do()
        {
            {
                auto pos1 = selectorPosition();
                auto direction = wantsDelta();

                if (pos1 != _lastPosition || direction != _lastDirection)
                {
                    if (onDo())
                    {
                        _cooldownTimer.Start(_cooldown);
                        _doEvents.Do();
                        return true;
                    }
                    return false;
                }
            }

            return Action::Do();
        }

        bool onDo() override
        {
            auto pos1 = selectorPosition();
            glm::ivec2 dir = wantsDelta();
            auto pos2 = pos1 + dir;

            if (!levelContainsPosition(pos1) || !levelContainsPosition(pos2))
                return false;

            auto direction = wantsDelta();

            if (direction == glm::ivec2{ 0,0 })
                return false;


            auto gem1 = level().at(pos1);
            auto gem2 = level().at(pos2);

            if (gem1 && !gem1->canBeMovedByPlayer())
                return false;
            if (gem2 && !gem2->canBeMovedByPlayer())
                return false;

            //dont allow to swap up/down with empty spaces
            {
                if (/*direction == Selector::Direction::Up ||*/ direction == glm::ivec2{ 0,1 })
                {
                    if (!gem1 || !gem2)
                        return false;
                }
            }


            level().SwapGems(pos1, pos2);

            _lastDirection = direction;
            _lastPosition = pos1;

            return true;
        }

        glm::ivec2 _lastPosition = { -1,-1 };
        glm::ivec2 _lastDirection = { 0, 0 };
    };

    class DestroyGemAction : public Action
    {
    public:
        bool onDo() override
        {
            auto pos = selectorPosition();
            if (!levelContainsPosition(pos))
                return false;

            level().DestroyGem(pos);
            return true;
        }
    };

    struct ActionCreator
    {
        static std::shared_ptr<Action> createSwap();
        static std::shared_ptr<Action> createFireball();
        static std::shared_ptr<Action> createFrostbolt();
        static std::shared_ptr<Action> createGemRain();
    };

    class ActionList
    {
    public:
        void Add(const Action::pointer& action)
        {
            _list.push_back(action);
        }

        auto& list() { return _list; }

        bool UseAction(int index);

        void UseAll()
        {
            for (auto& action : _list)
                action->Do();
        }

        void Update()
        {
            for (auto& action : _list)
                action->Update();
        }
    protected:
        std::vector<Action::pointer> _list;
        MX::Time::ManualStopWatchAbsolute   _actionCooldown;
    };
}


#endif
