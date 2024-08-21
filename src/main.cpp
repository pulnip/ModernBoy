#include <print>
#include <SDL2/SDL_main.h>
#include "Core.hpp"
#include "helper.hpp"

using namespace std;
using namespace ModernBoy;

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]){
    try{
        Core core{};
        core.run();
    }
    catch(Exception& e){
        println("{} ({}, {})", e.msg, e.fileName, e.lineNumber);
    }

	return 0;
}
