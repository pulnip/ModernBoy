#include <SDL2/SDL_image.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_render.h>

#include "SubEngine/GraphicsEngineWithSDL.hpp"

GraphicsEngineWithSDL::~GraphicsEngineWithSDL() {
    SDL_DestroyRenderer(*renderer);
    SDL_DestroyWindow(*window);
}

void GraphicsEngineWithSDL::postConstruct() noexcept {
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
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    if (*renderer == nullptr) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return;
    }

    // Init SDL Image Library
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return;
    }
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

void GraphicsEngineWithSDL::onNotify(ColorRect rect) noexcept {
    // transform position: center to top-left
    rect.position = rect.position - rect.size / 2;

    auto _rect = SDL_Rect{
        static_cast<int>(rect.position.x),
        static_cast<int>(rect.position.y),
        static_cast<int>(rect.size.x),
        static_cast<int>(rect.size.y),
    };

    SDL_SetRenderDrawColor(*renderer,
                           rect.color.red,
                           rect.color.green,
                           rect.color.blue,
                           rect.color.alpha);
    SDL_RenderFillRect(*renderer, &_rect);
}

void GraphicsEngineWithSDL::onNotify(SDL_Sprite sprite) noexcept {
    // transform position: center to top-left
    sprite.position = sprite.position - sprite.size / 2;

    auto _rect = SDL_Rect{
        static_cast<int>(sprite.position.x),
        static_cast<int>(sprite.position.y),
        static_cast<int>(sprite.size.x),
        static_cast<int>(sprite.size.y),
    };

    SDL_RenderCopyEx(*renderer,
                     sprite.texture,
                     // 텍스처의 특정 영역 (nullptr은 전체영역)
                     nullptr,
                     // 어느 위치에, 어느 크기로 렌더링할 지
                     &_rect,
                     // 라디안을 각도로 변환
                     Math::toDegree(sprite.rotation),
                     // 회전 중심점
                     nullptr,
                     SDL_FLIP_NONE);
}
