#include <set>
#include "System/Graphic/Graphics.hpp"

struct DrawOrder{
    using Sprite = Graphic::Painter::BasicSprite;
    bool operator()(const Sprite& lhs, const Sprite& rhs) const{
        lhs.drawOrder < rhs.drawOrder;
    }
};

void Graphics::initialize(const Graphic::Window& window) noexcept{
    windowSetting=window;
}

void Graphics::render(const Game::Time& lag) noexcept{
    clearScreen();

    std::multiset<BasicSprite, DrawOrder> buffer;
    for(const auto& sprite: basicSprites){
        buffer.emplace(sprite);
    }

    for(const auto& sprite: buffer){
        draw(sprite.box);
    }

    swapBuffer();
}

void Graphics::request(int id, const BasicSprite& sprite) noexcept{
    basicSprites.insert_or_assign(id, sprite);
}

void Graphics::remove(int id) noexcept{
    basicSprites.erase(id);
}