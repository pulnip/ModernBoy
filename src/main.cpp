#include <exception>
#include <print>
#include <SDL2/SDL_main.h>
#include "Core.hpp"

using namespace std;
using namespace ModernBoy;

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]){
    try{
        Core core{};
        core.run();
    }
    catch(const exception& e){
        println("{}", e.what());
    }

	return 0;
}
