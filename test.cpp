#include <iostream>
#include <fstream>

#include "Engine/Logger.hpp"
#include "JsonHelper.hpp"
#include "LevelLoader.hpp"

void print(const Blueprint::Actor& blueprint){
    const auto& position=blueprint.physicsAttribute.position.linear;
    const auto& size=blueprint.physicsAttribute.volume.base;
    const auto& color=blueprint.color;

    std::cout<<position.x<<", "<<position.y<<'\n';
    std::cout<<size.x<<", "<<size.y<<'\n';
    std::cout<<static_cast<int>(color.red)<<", "<<
        static_cast<int>(color.green)<<", "<<
        static_cast<int>(color.blue)<<", "<<
        static_cast<int>(color.alpha)<<'\n';
}

int main(void){
    Logger::Impl::base=std::make_shared<WithSTD::Logger>();
    Engine::LevelLoader ll;

    auto _doc=ll.loadJson("level0.json");
    if(not _doc.has_value()){
        return 1;
    }
    rapidjson::Document doc=std::move(_doc.value());

    auto _actors=JsonHelper::getArray(doc, "Actors");
    if(not _actors.has_value()){
        return 2;
    }
    auto actors=_actors.value();

    // iterating...
    for(auto& actor: actors){
        auto _bp=ActorHelper::getActor(actor);
        if(not _bp.has_value()){
            return 3;
        }
        auto blueprint=_bp.value();
        print(blueprint);
    }

    return 0;
}