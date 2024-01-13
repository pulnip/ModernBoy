#include <SDL2/SDL_main.h>

#include "GameEngine/p1pingpong.hpp"

int main(int argc, char *argv[]) {
    auto game = GameEngine::make<p1pingpong>();

    game->run();

    return 0;
}