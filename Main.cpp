#include <SDL2/SDL_main.h>

#include "p1pingpong.hpp"

int main(int argc, char *argv[]) {
    std::shared_ptr<IGameEngine> game = GameEngine::make<p1pingpong>();

    game->run();

    return 0;
}