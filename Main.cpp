#include "Game.hpp"

#include "SDL2/SDL.h"

int main(int argc, char* argv[]){
    auto game=std::make_shared<Game>();

    bool success = game->initialize(game);
    
    if(success){
        game->runLoop();
    }
    game->shutDown();

    return 0;
}