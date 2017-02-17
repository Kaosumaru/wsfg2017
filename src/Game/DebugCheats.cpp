#include "DebugCheats.h"
#include "Scene/Script/Event.h"
#include "Application/Window.h"
#include "Devices/Keyboard.h"
#include "Devices/Mouse.h"
#include "Scene/Sprites/ScriptableSpriteActor.h"

using namespace BH;
using namespace MX;

struct DebugNewArenaCheats_Data
{
    DebugNewArenaCheats_Data()
    {

    }

    ~DebugNewArenaCheats_Data()
    {

    }


    void UpdateWith(const DebugNewArenaCheats_Data &data)
    {
        *this = data;
    }

    void Clear()
    {
        UpdateWith(DebugNewArenaCheats_Data{});
        CleanOldBinding();
    }

    void Do()
    {
        if (CleanOldBinding())
            return;

        else if (_event)
            AddEvent();
        else if (_generic_object)
            AddObject();
    }


    auto& position() const
    {
        return MX::Window::current().mouse()->position();
    }

    EventPtr                               _event;
    std::shared_ptr<MX::ScriptObject>     _generic_object;

protected:
    bool CleanOldBinding()
    {
        return false;
    }

    void AddEvent() const
    {
        if (!Context<BaseGraphicScene>::isCurrent())
        {
            assert(false);
            return;
        }
            
        auto target = std::make_shared<ScriptableSpriteActor>();
        target->geometry.position = position();
        Context<BaseGraphicScene>::current().AddActor(target);

        {
            auto guard = Context<ScriptableSpriteActor>::Lock(target);
            _event->Do();
        }


        target->Unlink();
    }


    void AddObject()
    {

    }
};

namespace MX
{
    template<>
    struct PropertyLoader<DebugNewArenaCheats_Data>
    {
        using type = PropertyLoader_Standard;
        static bool load(DebugNewArenaCheats_Data& out, const Scriptable::Value::pointer& obj)
        {
            out.Clear();

            if (out._event = obj->to_object<Event>())
                return true;
            if (out._generic_object = obj->to_object<MX::ScriptObject>())
                return true;
            return true;
        }
    };

}



class DebugNewCheatObject : public CheatObject, public MX::SignalTrackable
{
public:
    using MapType = std::map<int, DebugNewArenaCheats_Data>;

    DebugNewCheatObject()
    {
        LoadData();
        Script::onParsed.static_connect(std::bind(&DebugNewCheatObject::LoadData, this));

		using namespace std::placeholders;

        MX::Window::current().keyboard()->on_char.connect(std::bind(&DebugNewCheatObject::OnKeyDown, this, _1), this);
    }

    void LoadData()
    {
        ScriptObjectString script("Game.Cheats.Bindings");


        std::map<std::string, DebugNewArenaCheats_Data> stringEventsKeys;
        script.load_property(stringEventsKeys, "Keys");
        script.load_property(stringEventsKeys, "Personal.Keys");


        MapType eventsKeys;
        for (auto &p : stringEventsKeys)
            eventsKeys[stringToKey(p.first)] = p.second;

        MapType diff;
        auto compare = [](auto a, auto b) { return a.first < b.first; };

        std::set_difference(_eventsKeys.begin(), _eventsKeys.end(), eventsKeys.begin(), eventsKeys.end(),
            std::inserter(diff, diff.begin()), compare);


        for (auto &p : diff)
            p.second.UpdateWith(DebugNewArenaCheats_Data{});

        for (auto &p : eventsKeys)
            _eventsKeys[p.first].UpdateWith(p.second);

    }

    char stringToKey(const std::string &key)
    {
        if (key.size() == 1)
            return key[0];
        return 0;
    }

    void OnKeyDown(int unichar)
    {
        auto it = _eventsKeys.find(unichar);
        if (it == _eventsKeys.end())
            return;
        auto &data = it->second;

        data.Do();
    }


protected:
    MapType _eventsKeys;
};

namespace BH
{
    std::shared_ptr<CheatObject> CreateCheats()
    {
        return std::make_shared<DebugNewCheatObject>();
    }
}


