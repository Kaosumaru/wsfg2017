#ifndef BHMANA
#define BHMANA
#include<memory>
#include<vector>
#include "utils/Utils.h"
#include "utils/SignalizingVariable.h"

namespace BH
{
    class Mana
    {
    public:
        using pointer = std::shared_ptr<Mana>;

        Mana(float max = 20)
        {
            _max = max;
        }

        SignalizingVariable<float> _current = 0.0f;
        SignalizingVariable<float> _max;

        void Gain(float amount)
        {
            auto new_current = _current + amount;
            if (new_current > _max)
                new_current = _max;
            _current = new_current;
        }

        bool Pay(float amount)
        {
            if (_current < amount)
                return false;
            _current -= amount;
            return true;
        }

        void onDestroyedGems(int amount)
        {
            static std::vector<int> points = { 0, 1, 2, 3, 5, 7 };
            Gain(points[amount]);
        }
    protected:
    };

    class Stats
    {
    public:
        Stats()
        {
            for (int i = 0; i < 5; i++)
                _manas.push_back(std::make_shared<Mana>());
        }

        auto &manas() { return _manas; }
       
    protected:
        std::vector<Mana::pointer> _manas;
    };
}


#endif
