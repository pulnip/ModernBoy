#include <SDL2/SDL_main.h>

#include "TestGame.hpp"

int main(int argc, char *argv[]) {
    std::shared_ptr<IGameEngine> game = GameEngine::make<TestGame>();

    game->run();

    return 0;
}