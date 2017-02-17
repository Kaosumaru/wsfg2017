#include "Level.h"



#include "Game.h"
#include "Rule.h"
#include <numeric>

using namespace BH;

bool Selector::Move(const glm::ivec2& delta)
{
    if (delta == glm::ivec2{0, 0})
    {
        return false;
    }

    auto new_position = pos() + delta;

    auto &level = Context<Level>::current();
    if (!level.containsPosition(new_position))
        return false;

    if (new_position.y == 0 || new_position.y == level.height() - 1)
        return false;

    position = new_position;

    return true;
}

void Selector::ForceMove(Direction direction)
{
    auto delta = deltaFromDirection(direction);
    auto new_position = pos() + delta;

    auto &level = Context<Level>::current();
    if (!level.containsPosition(new_position))
        return;
    position = new_position;
    return;
}

Level::Level()
{
    _rules = std::make_shared<ActionList>();
    _rules->Add(std::make_shared<GravityRule>());
    _rules->Add(std::make_shared<Match3Rule>());

    _movementRule = std::make_shared<LevelMovement>();
    _rules->Add(_movementRule);

    _gems.resize(_width * _height);
    //RandomizeAll();
    RandomizeHalf();
}

bool Level::InsertGem(const glm::ivec2& pos, const Gem::pointer& gem)
{
    gem->SetPosition(pos);
    at(pos) = gem;
    onCreatedGem(gem);
    return true;
}

bool Level::SwapGems(const glm::ivec2& pos1, const glm::ivec2& pos2)
{
    assert(pos1 != pos2);
    auto gem1 = at(pos1);
    auto gem2 = at(pos2);

    if (gem2)
        gem2->SetPosition(pos1);
    if (gem1)
        gem1->SetPosition(pos2);

    at(pos1) = gem2;
    at(pos2) = gem1;

    return true;
}

void Level::DestroyGem(const glm::ivec2& pos)
{
    auto gem = at(pos);
    if (!gem)
        return;
    gem->Destroy();
    at(pos) = nullptr;
}

void Level::RandomizeAll()
{

}

void Level::RandomizeHalf()
{
    for (int x = 0; x < _width; x++)
    for (int y = _height/2; y < _height; y++)
    {
        InsertRandomNonExplodingGem({ x, y });
    }
}

void Level::Update()
{
    if (_startTime == -1)
        _startTime = Context<MX::Time::Timer>::current().total_seconds();

    for (auto &gem : _gems)
    {
        if (!gem)
            continue;

        if (gem->Update())
            continue;

        DestroyGem(gem->pos());
    }

    _rules->UseAll();
}

int Level::nonExplodingColorAt(glm::ivec2 pos)
{
    static std::vector<int> allColors;
    if (allColors.empty())
    {
        allColors.resize(Gem::maxType);
        std::iota(allColors.begin(), allColors.end(), 0);
    }

    std::vector<int> availableColors = allColors;



    static std::vector<std::pair<int, int>> deltas =
    {
        { -2, -1 },
        { -1, 1 },
        { 1, 2 }
    };

    auto compareGems = [&](const Gem::pointer &g1, const Gem::pointer &g2)
    {
        if (!g1 || !g2)
            return;
        if (g1->type() != g2->type())
            return;

        auto &vec = availableColors;
        auto color = g1->type();
        vec.erase(std::remove(vec.begin(), vec.end(), color), vec.end());
    };

    {
        for (auto &x : deltas)
        {
            auto gem1 = at(pos + glm::ivec2{ x.first, 0 });
            auto gem2 = at(pos + glm::ivec2{ x.second, 0 });
            compareGems(gem1, gem2);
        }
    }

    {
        for (auto &y : deltas)
        {
            auto gem1 = at(pos + glm::ivec2{ 0, y.first });
            auto gem2 = at(pos + glm::ivec2{ 0, y.second });
            compareGems(gem1, gem2);
        }
    }

    assert(!availableColors.empty());
    auto color = Random::randomFrom(availableColors);
    return color;
}

void Level::InsertRandomNonExplodingGem(glm::ivec2 pos)
{
    auto color = nonExplodingColorAt(pos);
    auto gem = std::make_shared<Gem>(color);
    InsertGem(pos, gem);
}