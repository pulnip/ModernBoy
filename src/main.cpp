#include <print>
#include <SDL2/SDL_main.h>
#include "helper.hpp"
#include "Window.hpp"
#include "LevelLoader.hpp"

using namespace ModernBoy;

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]){
    LevelLoader levelLoader("settings/glob.json");
    auto wd=levelLoader.getWindowDesc();

    try{
        Window window(wd);

        while(window.run()){

        }
    }
    catch(Exception& e){
        std::println("{} ({}, {})", e.msg, e.fileName, e.lineNumber);
    }

	return 0;
}
