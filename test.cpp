#include "LevelLoader.hpp"
#include "MainEngine.hpp"

int main(void){
    MainEngine mainEngine;
    LevelLoader levelLoader(mainEngine);

    levelLoader.loadLevel(nullptr, "Level0.json");
}