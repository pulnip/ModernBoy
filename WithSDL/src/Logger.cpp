#include <map>
#include <SDL2/SDL_log.h>
#include "WithSDL/Logger.hpp"

using namespace WithSDL;

using Level=Logger::Level;

const std::string& toStr(Level level){
    static std::map<Level, std::string> levelMap={
        {Level::DEBUG,    "[DEBUG]"},
        {Level::INFO,     "[INFO]"},
        {Level::WARNING,  "[WARNING]"},
        {Level::ERROR,    "[ERROR]"},
        {Level::CRITICAL, "[CRITICAL]"}
    };
    return levelMap[level];
}

void WithSDL::Logger::log(Level level, const char msg[]) noexcept{
    if(not isShown(level)) return;
    SDL_Log((toStr(level) + msg).c_str());
}

void WithSDL::Logger::log(Level level, const int& msg) noexcept{
    if(not isShown(level)) return;
    SDL_Log((toStr(level) + std::to_string(msg)).c_str());
}

void WithSDL::Logger::log(Level level, const double& msg) noexcept{
    if(not isShown(level)) return;
    SDL_Log((toStr(level) + std::to_string(msg)).c_str());
}

void WithSDL::Logger::log(Level level,
    const std::string& type, const std::string& data,
    const char state[]
) noexcept{
    if(not isShown(level)) return;
    SDL_Log((toStr(level)+type+'('+data+"): "+state).c_str());
}

void WithSDL::Logger::log(Level level,
    const std::string& type, const int& data,
    const char state[]
) noexcept{
    if(not isShown(level)) return;
    SDL_Log((toStr(level)+type+'('+std::to_string(data)+"): "+state).c_str());
}

void WithSDL::Logger::log(Level level,
    const std::string& type, const double& data,
    const char state[]
) noexcept{
    if(not isShown(level)) return;
    SDL_Log((toStr(level)+type+'('+std::to_string(data)+"): "+state).c_str());
}
