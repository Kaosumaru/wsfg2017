#ifndef BHRULE
#define BHRULE
#include<memory>
#include "utils/Utils.h"
#include "action.h"
#include "utils/SignalizingVariable.h"

namespace BH
{
    class Rule : public Action
    {
    public:
        Rule() : Action("Rule") {}
        Rule(const std::string& objectName) : Action(objectName) {}
        Rule(float cooldown) : Action("Rule")
        {
            _cooldown = cooldown;
        }
    };

    class GravityRule : public Rule
    {
    public:
        GravityRule() : Rule(0.1f)
        {

        }
        bool onDo() override
        {
            auto &l = level();

            for (int y = level().height() - 1; y > 0; y--)
            {
                for (int x = 0; x < level().width(); x++)
                {
                    auto currentGem = l.at({ x,y });
                    auto upperGem = l.at({ x,y-1 });

                    if (currentGem || !upperGem || (upperGem && !upperGem->canBeMovedByAnything()))
                    {
                        if (upperGem)
                            upperGem->_falling = false;
                        continue;
                    }

                    //TODO check it
                    upperGem->_falling = true;
                    l.SwapGems({ x,y }, { x,y - 1 });

                }
            }

            int y = level().height() - 1;
            for (int x = 0; x < level().width(); x++)
            {
                auto currentGem = l.at({ x,y });
                if (currentGem)
                    currentGem->_falling = false;
            }

            return true;
        }
    };

    class Match3Rule : public Rule
    {
    public:
        Match3Rule() : Rule("Rule.Match3")
        {
            load_property(_matchEvents, "OnMatch.Events");
        }

        bool onDo() override
        {
            auto &l = level();

            bool foundExplosion = false;
            auto foundRange = [&](glm::ivec2 start, glm::ivec2 end, int size, const glm::ivec2& d, int color)
            {
                if (size < 3 || color < 0)
                    return;
                foundExplosion = true;

                for (int i = 0; i < size; i++)
                {
                    l.at(start)->_wantToExplode = true;
                    start += d;
                }

                destroyedGems(color, size);
            };

            {
                glm::ivec2 horizDelta = { 1,0 };
                for (int y = 0; y < level().height(); y++)
                {
                    int prevColor = -1;
                    int rangeStart = -1;
                    for (int x = 0; x < level().width() + 1; x++)
                    {
                        auto gem = l.at({ x,y });
                        int color = gem ? gem->type() : -1;

                        if (gem && (gem->_falling || !gem->canBeMovedByAnything()))
                            color = -1;

                        if (color == prevColor)
                            continue;

                        auto rangeEnd = x - 1;
                        int size = rangeEnd - rangeStart + 1;
                        foundRange({ rangeStart, y }, { rangeEnd, y }, size, horizDelta, prevColor);
                        rangeStart = x;
                        prevColor = color;
                    }
                }
            }

            {
                glm::ivec2 vertDelta = { 0,1 };
                for (int x = 0; x < level().width(); x++)
                {
                    int prevColor = -1;
                    int rangeStart = -1;
                    for (int y = 0; y < level().height() + 1; y++)
                    {
                        auto gem = l.at({ x,y });
                        int color = gem ? gem->type() : -1;

                        if (gem && (gem->_falling || !gem->canBeMovedByAnything()))
                            color = -1;

                        if (color == prevColor)
                            continue;

                        auto rangeEnd = y - 1;
                        int size = rangeEnd - rangeStart + 1;
                        foundRange({ x, rangeStart }, { x, rangeEnd }, size, vertDelta, prevColor);
                        rangeStart = y;
                        prevColor = color;
                    }
                }
            }

            bool found = false;
            for (auto &gem : l.gems())
            {
                if (gem && gem->_wantToExplode)
                {
                    if (gem->QueueDestruction())
                        found = true;
                }
                    
            }

            if (found)
            {
                _matchEvents.Do();
            }

            return true;
        }
    protected:
        MX::EventHolder   _matchEvents;
    };


    class LevelMovement : public Rule
    {
    public:
        LevelMovement() : Rule(6.0f)
        {

        }

        float calculateSpeed()
        {
            float max = 0.5;
            float min = 8.0f;
            float game_time = 4 * 60.0f;

            float current_time = Context<MX::Time::Timer>::current().total_seconds();
            float elapsed_time = current_time - level().startTime();

            float p = elapsed_time / game_time;
            if (p > 1.0f)
                p = 1.0f;

            return MX::lerp_pair({ min, max }, p);

        }

        bool onDo() override
        {
            if (first)
            {
                first = false;
                return true;
            }

            MoveUp();
            _cooldown = calculateSpeed();
            return true;
        }

        void MoveUp()
        {
            auto &l = level();

            {
                int y = 1;
                for (int x = 0; x < level().width(); x++)
                {
                    glm::ivec2 pos = { x, y };
                    auto gem = l.at(pos);
                    if (!gem)
                        continue;

                    auto gem2 = l.at({ x, y+1 });
                    if (!gem2)
                        continue;

                    reportPlayerLost();
                }
            }


            for (int y = 1; y < level().height(); y++)
            {
                for (int x = 0; x < level().width(); x++)
                {
                    glm::ivec2 pos = { x, y };
                    glm::ivec2 upper_pos = { x, y-1 };
                    l.SwapGems(pos, upper_pos);
                }
            }
            FillRow(level().height()-1);
            l.selector()->ForceMove(Selector::Direction::Up);
        }

        void FillRow(int row)
        {
            auto &l = level();
            for (int x = 0; x < level().width(); x++)
            {
                glm::ivec2 pos = { x, row };
                l.InsertRandomNonExplodingGem(pos);
            }

        }


        bool first = true;
    };
}


#endif
