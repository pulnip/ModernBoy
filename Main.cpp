#include "Game.hpp"

int main(int argc, char* argv[]){
    Game game;

    bool success = game.Initialize();
    
    if(success){
        game.RunLoop();
    }
    game.ShutDown();

    return 0;
}