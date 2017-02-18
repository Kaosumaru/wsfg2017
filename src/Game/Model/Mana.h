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

		Mana(float current, float max)
		{
			_current = current;
			_max = max;
		}

		float percent()
		{
			return _current.directValueAccess() / _max.directValueAccess();
		}

        SignalizingVariable<float> _current = 0.0f;
        SignalizingVariable<float> _max;

        void Gain(float amount)
        {
            auto new_current = _current + amount;
            if (new_current > _max)
                new_current = _max;
			if (new_current < 0.0f)
				new_current = 0.0f;
            _current = new_current;
        }

		void Lose(float amount)
		{
			Gain(-amount);
		}

        bool Pay(float amount)
        {
            if (_current < amount)
                return false;
            _current -= amount;
            return true;
        }

		void Reset(float current, float max)
		{
			_current = current;
			_max = max;
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

			_hp = std::make_shared<Mana>(100.0f, 100.0f);
        }

        auto &manas() { return _manas; }
		auto &hp() { return _hp; }
       
    protected:
        std::vector<Mana::pointer> _manas;
		Mana::pointer _hp;
    };
}


#endif
