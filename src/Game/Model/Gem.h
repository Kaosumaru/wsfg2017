#ifndef BHGEM
#define BHGEM
#include<memory>
#include "glm/vec2.hpp"
#include "utils/Utils.h"
#include "utils/SignalizingVariable.h"
#include <cmath>
namespace BH
{
    class Gem
    {
    public:
        friend class Level;

        Gem(int type);
        using pointer = std::shared_ptr<Gem>;

        static pointer random();

        static const int maxType = 5;

        auto position() { return (glm::ivec2)_pos.directValueAccess(); }
        auto& pos() { return _pos; }

        auto& type() { return _type; }

        MX::Signal<void(void)> onDestroyed;
        MX::Signal<void(void)> onStartedExploding;

        SignalizingVariable<bool> _falling = false;
        SignalizingVariable<bool> _frozen = false;
        SignalizingVariable<float> _exploding = 0.0f;
        bool _wantToExplode = false;

        bool canBeMovedByAnything()
        {
            return _exploding == 0.0f;
        }

        bool canBeMovedByPlayer()
        {
            return !_falling && !_frozen && _exploding == 0.0f;
        }

        bool Update()
        {
            if (_exploding > 0)
            {
                _exploding = _exploding - MX::Time::Timer::current().elapsed_seconds();
                if (_exploding <= 0.0f)
                {
                    return false;
                }
            }
            return true;
        }

        float animationSpeed()
        {
            return 0.3f;
        }

        float explosionPercent()
        {
            if (_exploding == 0.0f)
                return 0.0f;
            float max = animationSpeed();

            return std::sin((max - _exploding) / max);
        }

        bool QueueDestruction()
        {
            if (_exploding > 0.0f)
                return false;
            _exploding = animationSpeed();
            onStartedExploding();
            return true;
        }
    protected:
        void SetPosition(glm::ivec2 pos)
        {
            _pos = pos;
        }

        void Destroy()
        {
            onDestroyed();
        }

        SignalizingVariable<glm::ivec2> _pos;
        int _type = 0;
    };
}


#endif
