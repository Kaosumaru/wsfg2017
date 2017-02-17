#include "View.h"
#include "Game/Model/Rule.h"
#include "Scene/Sprites/ScriptableSpriteActor.h"

using namespace MX;
using namespace BH;
using namespace std;

class GemView : public MX::Widgets::ScriptLayouterWidget, public MX::SignalTrackable
{
public:
    GemView(const Gem::pointer& gem)
    {
        _gem = gem;
        SetLayouter("Game.Gem.Layouter");
        SetPosition(positionFromGem(gem));

        properties().SetValue("GemType", _gem->type());

        _gem->pos().onValueChanged.connect(
            [&](const glm::ivec2 &newPos, const glm::ivec2 &oldPos)
        {
            OnMoving(newPos, oldPos);
        }, this);

        _gem->onDestroyed.connect(
            [&]()
        {
            Unlink();
        }, this);

        _gem->onStartedExploding.connect(
            [&]()
        {
            OnStartedExploding();
        }, this);

        _gem->_frozen.onValueChanged.connect([&](const bool &v, const bool & old)
        {
            properties().SetValue("Frozen", v ? 1.0f : 0.0f);
        }, this);

    }

    static auto from(const Gem::pointer& gem)
    {
        return std::make_shared<GemView>(gem);
    }

	glm::vec2 positionFromGem(const Gem::pointer& gem)
    {
        return{ gem->position().x * 64.0f, gem->position().y * 64.0f };
    }

    void Run() override
    {
        auto g1 = Context<Gem>::Lock(_gem);
        MX::Widgets::ScriptLayouterWidget::Run();
    }

    void Draw(float x, float y) override
    {
        auto g1 = Context<Gem>::Lock(_gem);
        MX::Widgets::ScriptLayouterWidget::Draw(x, y);
    }

protected:

    void OnStartedExploding()
    {
        static std::shared_ptr<MX::SpriteActor> actor;
        if (!actor)
        {
            ScriptObjectString script("Game.Effects");
            script.load_property(actor, "NeonExplosion");
        }

        auto cloned = actor->clone();
        auto parent = SpriteSceneStackManager::manager_of(this)->scenesStack().top();

        auto pos = this->absolute_position();
        cloned->geometry.position = pos + this->dimensions() / 2.0f;

        parent->AddActor(cloned);
    }

    void OnMoving(glm::ivec2 newPos, glm::ivec2 oldPos)
    {
        SetPosition(positionFromGem(_gem));
    }

    Gem::pointer _gem;
};

class ActionView : public MX::Widgets::ScriptLayouterWidget
{
public:
    ActionView(const Player::pointer& player, const Action::pointer& action)
    {
        _player = player;
        _action = action;
        SetLayouter("Game.Action.Layouter");
    }

    void Run() override
    {
        properties().SetValue("Progress", _action->costProgress());
        MX::Widgets::ScriptLayouterWidget::Run();
    }

    void Draw(float x, float y) override
    {
        auto g2 = Context<Player>::Lock(_player);
        auto g1 = Context<Action>::Lock(_action);
        if (_action->drawer())
        {
            auto g = Context<Widgets::Drawer>::Lock(_action->drawer());
            MX::Widgets::ScriptLayouterWidget::Draw(x, y);
        }
        else
            MX::Widgets::ScriptLayouterWidget::Draw(x, y);
    }

protected:
    Action::pointer _action;
    Player::pointer _player;
};

ActionsView::ActionsView(const Player::pointer& player)
{
    _player = player;
    SetLayouter("Game.Actions.Layouter");

    bool first = true;
    for (auto& action : player->actions().list())
    {
        if (first)
        {
            first = false;
            continue;
        }

        if (action && action->passive())
            continue;

        auto view = std::make_shared<ActionView>(_player, action);
        AddNamedWidget("Action", view);
    }
}


LevelView::LevelView(const Player::pointer& player, const Level::pointer& level)
{
    _player = player;
    _level = level;
    SetLayouter("Game.Level.Layouter");

    auto selectorView = std::make_shared<SelectorView>(_level->selector());
    AddNamedWidget("Selector", selectorView);

    onNewLevel();

    _level->onCreatedGem.connect([&]
    (const Gem::pointer& gem) 
    {
        if (!gem)
            return;
        auto gemView = GemView::from(gem);
        AddNamedWidget("Gem", gemView);
    }, this);

    level->onEvent.connect([&]
    (glm::ivec2 pos, MX::EventHolder &e)
    {
        auto target = std::make_shared<MX::ScriptableSpriteActor>();

        target->geometry.position = absolute_position();
        target->geometry.position.x += pos.x * 64.0f + 32.0f;
        target->geometry.position.y += pos.y * 64.0f + 32.0f;
        target->geometry.position.y -= this->scroll().y;


        Context<BaseGraphicScene>::current().AddActor(target);

        {
            auto guard = Context<ScriptableSpriteActor>::Lock(target);
            e.Do();
        }


        target->Unlink();
    }, this);
}

void LevelView::Draw(float x, float y)
{
    auto g1 = Context<Player>::Lock(_player);
    MX::Widgets::ScriptLayouterWidget::Draw(x, y);
}

void LevelView::Run()
{
    int scroll = _level->movementRule()->cooldownTimer().percent() * 64.0f;
    SetVerticalScroll(scroll);
    MX::Widgets::ScriptLayouterWidget::Run();
}

void LevelView::onNewLevel()
{
    for (auto& gem : _level->gems())
    {
        if (!gem)
            continue;

        auto gemView = GemView::from(gem);
        AddNamedWidget("Gem", gemView);
    }

}







SelectorView::SelectorView(const Selector::pointer& selector)
{
    _selector = selector;
    SetLayouter("Game.Selector.Layouter");
    SetPosition(positionFromSelector());

    _selector->position.onValueChanged.connect([&](auto&...)
    {
        SetPosition(positionFromSelector());
    }, this);
}

glm::vec2 SelectorView::positionFromSelector()
{
    int x = _selector->pos().x;
    int y = _selector->pos().y;
    return { x * 64.0f, y * 64.0f };
}