#ifndef __INC_APP_STATE_HPP
#define __INC_APP_STATE_HPP

#include <SDL3/SDL_render.h>
#include "game_context.hpp"

struct AppState{
    SDL_Window* window;
    struct DX11RenderState* renderState;
    GameContext game_ctx;
    Uint64 last_step;
};

#endif // __INC_APP_STATE_HPP