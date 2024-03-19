#include "Engine/PhysicsSimulator.hpp"
#include "Engine/Graphics.hpp"
#include "Engine/InputSystem.hpp"

#include <map>
#include "Engine/JsonHelper.hpp"
#include "Math.hpp"
#include "Component/Controllable.hpp"
#include "Component/Drawable.hpp"
#include "Component/Movable.hpp"

using namespace My::Math;
using namespace JsonHelper;
using namespace CustomHelper;
using namespace ActorHelper;

std::optional<int> JsonHelper::getInt(
    const rapidjson::Value& object, const std::string& property
) noexcept{
    auto it=object.FindMember(property.c_str());

    if(it == object.MemberEnd() or not it->value.IsInt()){
        return std::nullopt;
    }

    return it->value.GetInt();
}

std::optional<double> JsonHelper::getDouble(
    const rapidjson::Value& object, const std::string& property
) noexcept{
    auto it=object.FindMember(property.c_str());

    if(it==object.MemberEnd() or not it->value.IsDouble()){
        return std::nullopt;
    }

    return it->value.GetDouble();
}

std::optional<std::string> JsonHelper::getString(
    const rapidjson::Value& object, const std::string& property
) noexcept{
    auto it=object.FindMember(property.c_str());

    if(it==object.MemberEnd() or not it->value.IsString()){
        return std::nullopt;
    }

    return it->value.GetString();
}

std::optional<JsonHelper::Array> JsonHelper::getArray(
    const rapidjson::Value& object, const std::string& property
) noexcept{
    auto it=object.FindMember(property.c_str());

    if(it==object.MemberEnd() or not it->value.IsArray()){
        return std::nullopt;
    }

    return it->value.GetArray();
}

std::optional<rapidjson::GenericObject<true, rapidjson::Value>> JsonHelper::getObject(
    const rapidjson::Value& object, const std::string& property
) noexcept{
    auto it=object.FindMember(property.c_str());

    if(it==object.MemberEnd() or not it->value.IsObject()){
        return std::nullopt;
    }

    return it->value.GetObject();
}

std::optional<Game::Vector2D> CustomHelper::getVector2D(
    const rapidjson::Value& object, const std::string& property
) noexcept{
    auto it=object.FindMember(property.c_str());
    if(it==object.MemberEnd() or not it->value.IsObject()){
        return std::nullopt;
    }

    auto vector2d=it->value.GetObject();

    auto x=getDouble(vector2d, "x");
    auto y=getDouble(vector2d, "y");
    if(not x.has_value() or not y.has_value()){
        return std::nullopt;
    }

    return Game::Vector2D{x.value(), y.value()};
}

std::optional<Skin::TrueColor> CustomHelper::getColor(
    const rapidjson::Value& object, const std::string& property
) noexcept{
    auto it=object.FindMember(property.c_str());
    if(it==object.MemberEnd() or not it->value.IsObject()){
        return std::nullopt;
    }

    auto color=it->value.GetObject();

    auto red=getInt(color, "red");
    auto green=getInt(color, "green");
    auto blue=getInt(color, "blue");
    auto alpha=getInt(color, "alpha");
    if( not red.has_value() or
        not green.has_value() or
        not blue.has_value() or
        not alpha.has_value()
    ){
        return std::nullopt;
    }

    return Skin::TrueColor{
        static_cast<unsigned char>(red.value()),
        static_cast<unsigned char>(green.value()),
        static_cast<unsigned char>(blue.value()),
        static_cast<unsigned char>(alpha.value())
    };
}

enum class Control{
    MOVE
};

enum class Direction{
    UPWARD, DOWNWARD, LEFTWARD, RIGHTWARD
};

std::optional<std::shared_ptr<Actor::Vanilla>> ActorHelper::getActor(
    const rapidjson::Value& object
) noexcept{
    auto actor=Actor::Vanilla::make<Actor::Vanilla>();

    auto role=getString(object, "type");

    if(role){
        static std::map<std::string, Actor::Role> roleMap={
            {"Object", Actor::Role::Neutral},
            {"Player", Actor::Role::Player},
            {"Enemy", Actor::Role::Enemy}
        };

        actor->setRole(roleMap[role.value()]);
    }

    auto position=getVector2D(object, "position");
    auto size=getVector2D(object, "size");
    auto velocity=getVector2D(object, "velocity");

    if(position && size){
        auto movable=std::make_shared<Component::Movable>(
            actor, Skin::Attribute_2D(position.value(), size.value())
        );

        if(velocity){
            movable->get().velocity.linear=velocity.value();
        }

        actor->add(movable);
#warning "move to movable postconstruct"
        Engine::PhysicsSimulator::get()->append(actor);
    }

    auto color=getColor(object, "color");

    if(color){
        auto colored=std::make_shared<Component::Colored>(
            actor, color.value()
        );
        actor->add(colored);
        Engine::Graphics::get()->append(colored);
    }

    auto qControls=getArray(object, "controls");
    if(qControls){
        auto controllable=std::make_shared<Component::Controllable>(actor);
        actor->add(controllable);

        auto controls=qControls.value();
        for(auto it=controls.Begin(); it!=controls.End(); ++it){
            auto type=getString(*it, "type");
            auto key=getString(*it, "key");
            
            if(not type || not key) break;

            auto detail=getObject(*it, "detail");

            static std::map<std::string, Control> typeMap={
                {"move", Control::MOVE}
            };

            switch(typeMap[type.value()]){
            case Control::MOVE:
                auto direction=getString(*detail, "direction");
                auto speed=getDouble(*detail, "speed");

                if(direction && speed){
                    static std::map<std::string, Direction> dtypeMap={
                        {"upward", Direction::UPWARD},
                        {"downward", Direction::DOWNWARD},
                        {"leftward", Direction::LEFTWARD},
                        {"rightward", Direction::RIGHTWARD}
                    };

                    Engine::InputSystem::get()->registerKey(key.value()[0]-93, controllable);
                    
                    switch(dtypeMap[direction.value()]){
                      case Direction::UPWARD:
                        controllable->setKey(key.value()[0]-93,
                            std::make_shared<Component::Behaviour::MoveY>(
                                actor, -speed.value()
                            ),
                            std::make_shared<Component::Behaviour::MoveY>(
                                actor, 0
                            )
                        );
                        break;
                      case Direction::DOWNWARD:
                        controllable->setKey(key.value()[0]-93,
                            std::make_shared<Component::Behaviour::MoveY>(
                                actor, +speed.value()
                            ),
                            std::make_shared<Component::Behaviour::MoveY>(
                                actor, 0
                            )
                        );
                        break;
                      case Direction::LEFTWARD:
                        controllable->setKey(key.value()[0]-93,
                            std::make_shared<Component::Behaviour::MoveX>(
                                actor, -speed.value()
                            ),
                            std::make_shared<Component::Behaviour::MoveX>(
                                actor, 0
                            )
                        );
                      case Direction::RIGHTWARD:
                        controllable->setKey(key.value()[0]-93,
                            std::make_shared<Component::Behaviour::MoveX>(
                                actor, speed.value()
                            ),
                            std::make_shared<Component::Behaviour::MoveX>(
                                actor, 0
                            )
                        );
                    }
                }
            }
        }
    }

    return actor;
}
