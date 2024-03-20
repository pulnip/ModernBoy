#include <map>
#include <iostream>
#include "WithSTD/Logger.hpp"

using namespace Logging;

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

void WithSTD::Logger::log(Level level,
    const Info& info, const std::string& msg
){
    if(not isShown(level)) return;
    std::cout<<toStr(level)<<info.type<<'('<<info.name<<"): "<<msg<<'\n';
}

void WithSTD::Logger::log(Level level,
    const Info& info, const String& data
){
    if(not isShown(level)) return;
    std::cout<<toStr(level)<<
        info.type<<'('<<info.name<<"): "<<
        data.type<<'('<<data.data<<")\n";
}

void WithSTD::Logger::log(Level level,
    const Info& info, const Integer& data
){
    if(not isShown(level)) return;
    std::cout<<toStr(level)<<
        info.type<<'('<<info.name<<"): "<<
        data.type<<'('<<data.data<<")\n";
}

void WithSTD::Logger::log(Level level,
    const Info& info, const Double& data
){
    if(not isShown(level)) return;
    std::cout<<toStr(level)<<
        info.type<<'('<<info.name<<"): "<<
        data.type<<'('<<data.data<<")\n";
}
