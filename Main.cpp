#include <memory>

#include <SDL2/SDL_main.h>

#include "MainEngine.hpp"
#include "TestGame.hpp"
#include "SubEngine/Logger.hpp"

int main(int argc, char *argv[]){
    auto m=std::make_shared<MainEngine>();
    const std::weak_ptr<MainEngine> wp=m;

    std::shared_ptr<Game::SubEngine::Logger> logger=
        std::make_shared<WithSTD::SubEngine::Logger_default>();

    auto game = Game::Core::Engine::make<TestGame>(wp, logger);

    game->run();

    return 0;
}