#include <cassert>
#include "Engine/Core.hpp"
#include "Engine/Logger.hpp"
#include "Component/Movable.hpp"
#include "WithSDL/Graphics.hpp"
#include "WithSDL/ResourceManager.hpp"
#include "WithSDL/BGSprite.hpp"
#include "Test/CelestialSky.hpp"

using namespace WithSDL;
using namespace Test;

void CelestialSky::initAbility() noexcept{
    using namespace Component;
    auto self=weak_from_this();

    auto movable=std::make_shared<Movable>(self);
    add(movable);

    auto textures_f=std::dynamic_pointer_cast<ResourceManager>(
        ResourceManager::get()
    )->getTexture(
        std::vector<std::string>{
            "Farback01.png",
            "Farback02.png"
        }
    );
    
    if(textures_f.has_value()){
        auto farbackBG=std::make_shared<BGSprite>(self);
        add(farbackBG);
        farbackBG->setScreenSize(Graphics::get()->getWindowSize());
        farbackBG->setScrollSpeed(-100.0f);

        farbackBG->setBGTextures(textures_f.value());

        Graphics::get()->append(farbackBG);
    }

    auto textures_c=std::dynamic_pointer_cast<ResourceManager>(
        ResourceManager::get()
    )->getTexture(
        std::vector<std::string>{
            "Stars.png",
            "Stars.png"
        }
    );
    
    if(textures_c.has_value()){
        auto closerBG=std::make_shared<BGSprite>(self);
        add(closerBG);
        closerBG->setScreenSize(Graphics::get()->getWindowSize());

        closerBG->setScrollSpeed(-200.0f);
        closerBG->setBGTextures(textures_c.value());

        Graphics::get()->append(closerBG);
    }
}