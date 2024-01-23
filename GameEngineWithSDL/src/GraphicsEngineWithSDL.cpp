#include <SDL2/SDL_image.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_render.h>

#include "Skin.hpp"
#include "GameEngine/GameEngine.hpp"
#include "GraphicsEngineWithSDL.hpp"

GraphicsEngineWithSDL::~GraphicsEngineWithSDL() {
    SDL_DestroyRenderer(*renderer);
    SDL_DestroyWindow(*window);
}

void GraphicsEngineWithSDL::onNotify(ColorRect r) noexcept {
    // #error "Not linked"
    SDL_Log("DRAW");
    // transform position: center to top-left
    r.rect.position = r.rect.position - r.rect.size / 2;

    auto _rect = SDL_Rect{
        static_cast<int>(r.rect.position.x),
        static_cast<int>(r.rect.position.y),
        static_cast<int>(r.rect.size.x),
        static_cast<int>(r.rect.size.y),
    };
    
    SDL_SetRenderDrawColor(*renderer,
                           r.color.red,
                           r.color.green,
                           r.color.blue,
                           r.color.alpha);
    SDL_RenderFillRect(*renderer, &_rect);
}

void GraphicsEngineWithSDL::setAttribute() noexcept {
    window = std::make_shared<SDL_Window *>(SDL_CreateWindow(
        "GameEngine Programming in C++",
        100,
        100,
        1024,
        768,
        0));
    if (*window == nullptr) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return;
    }

    renderer = std::make_shared<SDL_Renderer *>(SDL_CreateRenderer(
        *window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    ));
    if (*renderer == nullptr) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return;
    }

    // Init SDL Image Library
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return;
    }

    // SDL_SetRenderDrawBlendMode(*renderer,
    //     SDL_BLENDMODE_BLEND
    // );
}

void GraphicsEngineWithSDL::onNotify(SpriteForSDL sprite) noexcept{
    // transform position: center to top-left
    sprite.spinRect.rect.position =
        sprite.spinRect.rect.position - sprite.spinRect.rect.size / 2;

    auto _rect = SDL_Rect{
        static_cast<int>(sprite.spinRect.rect.position.x),
        static_cast<int>(sprite.spinRect.rect.position.y),
        static_cast<int>(sprite.spinRect.rect.size.x),
        static_cast<int>(sprite.spinRect.rect.size.y),
    };

    SDL_RenderCopyEx(*renderer,
                     sprite.texture,
                     // 텍스처의 특정 영역 (nullptr은 전체영역)
                     nullptr,
                     // 어느 위치에, 어느 크기로 렌더링할 지
                     &_rect,
                     // 라디안을 각도로 변환
                     Math::toDegree(sprite.spinRect.rotation),
                     // 회전 중심점
                     nullptr,
                     SDL_FLIP_NONE);
}

void GraphicsEngineWithSDL::initBackground() noexcept {
    // 후면 버퍼를 단색으로 클리어
    SDL_SetRenderDrawColor(*renderer,
                           0, 0, 255, // 파란 배경
                           255);

    SDL_RenderClear(*renderer);
}

void GraphicsEngineWithSDL::changeColorBuffer() noexcept {
    // 전면 버퍼와 후면 버퍼 교환
    SDL_RenderPresent(*renderer);
}


