#include <cassert>

#include "SubEngine/ActorManager.hpp"
#include "GraphicsEngineWithSDL.hpp"
#include "ResourceManagerWithSDL.hpp"
#include "CelestialSky.hpp"
#include "Component/Movable.hpp"
#include "BGSprite.hpp"

void CelestialSky::injectDependency() noexcept{
    auto self=weak_from_this();

    Component::make<Movable>(self);

    assert(!owner.expired());
    auto query=owner.lock()->query(SubEngine::Type::ResourceManager);
    assert(query.has_value());
    auto rm=std::dynamic_pointer_cast<ResourceManagerWithSDL>(query.value());

    // Far-Back background
    auto fbg=Component::make<BGSprite>(self);

    fbg->setScreenSize(Vector2{1024.0f, 768.0f});
    fbg->setScrollSpeed(-100.0f);

    auto img1=rm->getTexture("Farback01.png");
    assert(img1.has_value());
    auto img2=rm->getTexture("Farback02.png");
    assert(img2.has_value());

    fbg->setBGTextures(std::vector<SDL_Texture*>{img1.value(), img2.value()});

    // Closer background
    auto cbg=Component::make<BGSprite>(self);

    cbg->setScreenSize(Vector2{1024.0f, 768.0f});
    cbg->setScrollSpeed(-200.0f);
    std::static_pointer_cast<Component>(cbg)->setUpdateOrder(101);

    auto img3=rm->getTexture("Stars.png");
    assert(img3.has_value());

    cbg->setBGTextures(std::vector<SDL_Texture*>{img3.value(), img3.value()});
}