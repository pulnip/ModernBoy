#include "Game.hpp"

#include "SDL2/SDL.h"

int main(int argc, char* argv[]){
    auto game=Game::Factory::make<Game>();
    
    game->runLoop();

    game->shutDown();
    return 0;
}