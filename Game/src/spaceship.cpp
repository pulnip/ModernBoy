#include "spaceship.hpp"

void spaceShip::postConstruct() noexcept {
    GameEngineWithSDL::postConstruct();
#ifdef MUSI
    auto ship = Actor::make<Ship>(self);

    // Create actor for the background (this doesn't need a subclass)
    auto bgActor = Actor::make<Actor>(self);
    bgActor->position = Vector2{512.0f, 384.0f};
    // Create the "far back" background
    auto fbg = Component::Factory::make<BGSpriteComponent>(bgActor);
    fbg->setScreenSize(Vector2{1024.0f, 768.0f});
    std::vector<SDL_Texture *> fbgtexs = {
        getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Farback01.png"),
        getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Farback02.png")};
    fbg->setBGTextures(fbgtexs);
    fbg->setScrollSpeed(-100.0f);
    // Create the closer background
    auto cbg = Component::Factory::make<BGSpriteComponent>(bgActor);
    cbg->setUpdateOrder(101);
    cbg->setScreenSize(Vector2{1024.0f, 768.0f});
    std::vector<SDL_Texture *> cbgtexs = {
        getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Stars.png"),
        getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Stars.png")};
    cbg->setBGTextures(cbgtexs);
    cbg->setScrollSpeed(-200.0f);

    for (int i = 0; i < 20; ++i) {
        auto asteroid = Actor::make<Asteroid>(self);
    }
#endif
}