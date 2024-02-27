#include <map>
#include <iostream>
#include "Engine/Logger.hpp"

using namespace Logger;

namespace WithSTD{
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
}

void WithSTD::Logger::log(Level level, const char msg[]) noexcept{
    if(not isShown(level)) return;
    std::cout<<toStr(level)<<msg<<'\n';
}

void WithSTD::Logger::log(Level level, const int& msg) noexcept{
    if(not isShown(level)) return;
    std::cout<<toStr(level)<<msg<<'\n';
}

void WithSTD::Logger::log(Level level, const double& msg) noexcept{
    if(not isShown(level)) return;
    std::cout<<toStr(level)<<msg<<'\n';
}

void WithSTD::Logger::log(Level level,
    const std::string& type, const std::string& data,
    const char state[]
) noexcept{
    if(not isShown(level)) return;
    std::cout<<toStr(level)<<type<<'('<<data<<"): "<<state<<'\n';
}

void WithSTD::Logger::log(Level level,
    const std::string& type, const int& data,
    const char state[]
) noexcept{
    if(not isShown(level)) return;
    std::cout<<toStr(level)<<type<<'('<<data<<"): "<<state<<'\n';
}

void WithSTD::Logger::log(Level level,
    const std::string& type, const double& data,
    const char state[]
) noexcept{
    if(not isShown(level)) return;
    std::cout<<toStr(level)<<type<<'('<<data<<"): "<<state<<'\n';
}
