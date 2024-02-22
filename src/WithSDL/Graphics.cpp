#include "WithSDL/Graphics.hpp"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>

#include "Math.hpp"
#include "Skin.hpp"
#include "Engine/Core.hpp"
#include "Engine/Logger.hpp"

using namespace WithSDL;
using namespace My::Math;

Graphics::Graphics() noexcept:
    logger(std::make_unique<Engine::BindedLogger>("Graphics", "WithSDL"))
{
    logger->debug("constructed");
}

Graphics::~Graphics(){
    logger->debug("destructed");
}

static auto& title=Engine::title;
static auto& screen=Engine::screen;

void Graphics::initialize() noexcept{
    logger->debug("try to create window");
    window=SDL_CreateWindow(
        title.c_str(),
        screen.x, screen.y,
        screen.width, screen.height,
        0
    );
    if(window==nullptr){
        logger->info("failed to create window");
        logger->info(SDL_GetError());
        return;
    } else{
        logger->debug("creating window succeed");
    }

    logger->debug("try to create renderer");
    renderer=SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if(renderer==nullptr){
        logger->info("failed to create renderer");
        logger->info(SDL_GetError());
        return;
    } else{
        logger->debug("creating renderer succeed");
    }

    // Init SDL Image Library
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        logger->info("Unable to initialize SDL_image");
        logger->info(SDL_GetError());
        return;
    } else{
        logger->debug("initializing SDL_image succeed");
    }

    // SDL_SetRenderDrawBlendMode(*renderer,
    //     SDL_BLENDMODE_BLEND
    // );
}

void Graphics::destroyAll() noexcept{
    SDL_DestroyRenderer(renderer);
    logger->debug("renderer destructed");
    SDL_DestroyWindow(window);
    logger->debug("window destructed");
}

void* Graphics::context() noexcept{
    logger->debug("get context");
    return renderer;
}

void Graphics::draw(ColorRect object) noexcept{
    logger->debug("draw ColorRect");
    Vector2<int> pos=object.rect.position; 
    Vector2<int> size=object.rect.size;
    ::Skin::TrueColor color=object.color;
    // transform position: center to top-left
    auto transformed = pos - size/2;

    auto rect=SDL_Rect{
        transformed.x, transformed.y,
        size.x, size.y
    };

    logger->debug("set color");
    SDL_SetRenderDrawColor(renderer,
        color.red, color.green, color.blue,
        color.alpha
    );

    logger->debug("fill rect");
    SDL_RenderFillRect(renderer, &rect);
}

void Graphics::draw(Sprite object) noexcept{
    logger->debug("draw sprite");
    Vector2<int> pos=object.spinRect.rect.position; 
    Vector2<int> size=object.spinRect.rect.size;
    auto angle=object.spinRect.rotation;
    SDL_Texture* texture=object.texture;
    // transform position: center to top-left
    auto transformed = pos - size/2;
    auto rect=SDL_Rect{
        transformed.x, transformed.y,
        size.x, size.y
    };
    logger->debug("render sprite");
    SDL_RenderCopyEx(renderer,
        texture, nullptr,
        &rect,
        angle.toDegree().get(), nullptr,
        SDL_FLIP_NONE
    );
}
void Graphics::clearScreen() noexcept{
    logger->debug("clear screen");
    // 후면 버퍼를 단색으로 클리어
    SDL_SetRenderDrawColor(renderer,
        0, 0, 255, // 파란 배경
        255
    );
    SDL_RenderClear(renderer);
}
void Graphics::swapBuffer() noexcept{
    logger->debug("swap buffer");
    SDL_RenderPresent(renderer);
}
