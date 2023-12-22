#include <SDL2/SDL.h>

#include "Game.hpp"

int main(int argc, char* argv[]){
    auto game=Game::Factory::make<p1pingpong>();
    
    game->runLoop();

    game->shutDown();
    return 0;
}