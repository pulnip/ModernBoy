#include <SDL2/SDL.h>

#include "Game.hpp"

int main(int argc, char *argv[]) {
    auto game = GameEngine::make<p1pingpong>();

    game->run();

    return 0;
}