#include <SDL2/SDL_main.h>

#include "TestGame.hpp"
#include "Logger.hpp"

int main(int argc, char *argv[]) {
    std::shared_ptr<Game::Plugin::Logger> logger=
        std::make_shared<WithSTD::Plugin::Logger_default>();
    std::shared_ptr<IGameEngine> game = GameEngine::make<TestGame>(
        logger
    );

    game->run();

    return 0;
}