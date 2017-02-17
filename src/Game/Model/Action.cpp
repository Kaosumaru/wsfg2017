#include "Action.h"

#include "Game.h"
#include "Player.h"
#include "widgets/Drawers/Drawer.h"

using namespace BH;



Action::Action(const std::string& objectName) : MX::ScriptObjectString(objectName)
{
    load_property(_passive, "Passive");
    load_property(_cooldown, "Cooldown");
    load_property(_manaCost, "ManaCost");
    load_property(_doEvents, "Events");
    load_property(_gemEvents, "Gem.Events");
    load_property(_drawer, "Drawer");

    int manaSource = -1;
    load_property(manaSource, "ManaSource");
    if (manaSource != -1)
        SetManaSource(manaSource);
}

bool Action::Do()
{
    if (!_cooldownTimer.Tick())
        return false;
    if (_manaSource && !_manaSource->Pay(_manaCost))
        return false;

    if (onDo())
    {
        _doEvents.Do();

        if (_selectedGemPos.x != -1)
        {
            if (_selectedGemPosAtEnemy)
                enemyLevel().onEvent(_selectedGemPos, _gemEvents);
            else
                level().onEvent(_selectedGemPos, _gemEvents);
        }
            

        if (_cooldown != 0.0f)
            _cooldownTimer.Start(_cooldown);
        return true;
    }
    return false;
}

Player& Action::enemyPlayer()
{
    auto &game = Context<Game>::current();

    auto &current = Context<Player>::current();;

    for (auto &player : game.players())
    {
        if (&current != player.get())
            return *player;
    }
    return current;
}
Level &Action::enemyLevel()
{
    return *(enemyPlayer().level());
}

void Action::SetManaSource(int i)
{
    auto &player = Context<Player>::current();
    SetManaSource(player.stats().manas()[i]);
}

glm::ivec2 Action::selectorPosition()
{
    auto &player = Context<Player>::current();
    return player.selectorPosition();
}

void Action::destroyedGems(int color, int size)
{
    auto &player = Context<Player>::current();
    player.stats().manas()[color]->onDestroyedGems(size);
}

bool Action::levelContainsPosition(const glm::ivec2& pos)
{
    auto &level = Context<Level>::current();
    return level.containsPosition(pos);
}

void Action::reportPlayerLost()
{
    auto& player = Context<Player>::current();
    player._lost = true;
}

glm::ivec2 Action::wantsDelta()
{
    auto &player = Context<Player>::current();
    return player.schema().direction.target;
}

bool ActionList::UseAction(int index)
{
    auto &player = Context<Player>::current();

    float cooldown = 0.1f;
    //if (!_actionCooldown.Tick())
    //    return false;

    if (index < (int)list().size())
    {
        auto &action = list()[index];
        if (!action)
            return false;
        action->Do();
        _actionCooldown.Start(cooldown);
        return true;
    }
    return false;
}

class FireballAction : public Action
{
public:
    FireballAction(const std::string& objectName) : Action(objectName)
    {
    }

    bool onDo() override
    {
        auto pos = selectorPosition();
        _selectedGemPos = pos;
        enumerateBlocks3x3(pos, [&](auto &level, auto& pos, auto &gem)
        {
            if (gem)
                gem->QueueDestruction();
        });

        return true;
    }
};

class FrostboltAction : public Action
{
public:
    FrostboltAction(const std::string& objectName) : Action(objectName)
    {

    }

    bool onDo() override
    {
        //auto pos = selectorPosition();
        auto pos = bestPoint();
        freezeAt(pos);
        return true;
    }

    glm::ivec2 bestPoint()
    {
        int maxPoints = -1;
        glm::ivec2 res = selectorPosition();
        int count = 20;

        for (int i = 0; i < count; i++)
        {
            glm::ivec2 p = randomPosition(1);
            int points = calculatePoints(p);
            if (points <= maxPoints)
                continue;
            maxPoints = points;
            res = p;
        }
        return res;
    }

    void freezeAt(const glm::ivec2& pos)
    {
        _selectedGemPos = pos;
        _selectedGemPosAtEnemy = true;

        enumerateEnemyBlocks3x3(pos, [](auto &level, auto& pos, auto &gem)
        {
            if (gem)
                gem->_frozen = true;
        });
    }

    int calculatePoints(const glm::ivec2& pos)
    {
        int points = 0;
        enumerateEnemyBlocks3x3(pos, [&](auto &level, auto& pos, auto &gem)
        {
            points += gem && !gem->_frozen ? 1 : 0;
        });
        return points;
    }
};


class GemRainAction : public Action
{
public:
    GemRainAction(const std::string& objectName) : Action(objectName)
    {

    }

    bool Do() override
    {
        return Action::Do();
    }

    bool onDo() override
    {
        auto depths = getDepths();
        std::sort(depths.begin(), depths.end(), []( auto &a, auto &b)
        {
            return a.y > b.y;
        });

        auto gemsToCreate = _createGems;

        for (auto &d : depths)
        {
            if (d.y <= 1)
                break;
            if (gemsToCreate == 0)
                break;

            createGemAtRow(d.x, d.y - 1);

            gemsToCreate--;
        }

        return true;
    }

    void createGemAtRow(int x, int y)
    {
        auto color = enemyLevel().nonExplodingColorAt({ x, y });
        auto gem = std::make_shared<Gem>(color);
        enemyLevel().InsertGem({ x, 0 }, gem);
    }

    std::vector<glm::ivec2> getDepths()
    {
        std::vector<glm::ivec2> result;

        for (int x = 0; x < enemyLevel().width(); x++)
            result.push_back({x, depthForColumn(x)});

        return result;
    }

    int depthForColumn(int x)
    {
        int y = 0;
        int depth = 0;

        while (enemyLevel().at({ x, y++ }) == nullptr)
            depth++;

        return depth;
    }
protected:
    int _createGems = 3;
};

std::shared_ptr<Action> ActionCreator::createSwap()
{
    return std::make_shared<SwapGemsAction>("Actions.Swap");
}

std::shared_ptr<Action> ActionCreator::createFireball()
{
    return std::make_shared<FireballAction>("Actions.Fireball");
}

std::shared_ptr<Action> ActionCreator::createFrostbolt()
{
    return std::make_shared<FrostboltAction>("Actions.Frostbolt");
}

std::shared_ptr<Action> ActionCreator::createGemRain()
{
    return std::make_shared<GemRainAction>("Actions.GemRain");
}