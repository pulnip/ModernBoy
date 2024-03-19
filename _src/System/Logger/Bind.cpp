#include "System/Logging/Bind.hpp"
#include "Engine/SystemLocator.hpp"

using namespace Logging;

void Bind::log(const std::string& msg){
    auto lv=__Logger().getCurrLevel();
    __Logger().log(lv, _info, msg);
}

void Bind::log(const String& data){
    auto lv=__Logger().getCurrLevel();
    __Logger().log(lv, _info, data);
}

void Bind::log(const Integer& data){
    auto lv=__Logger().getCurrLevel();
    __Logger().log(lv, _info, data);
}

void Bind::log(const Double& data){
    auto lv=__Logger().getCurrLevel();
    __Logger().log(lv, _info, data);
}

void Bind::debug(const std::string& msg){
    __Logger().log(Logging::Level::DEBUG, _info ,msg);
}

void Bind::info(const std::string& msg){
    __Logger().log(Logging::Level::INFO, _info ,msg);
}

void Bind::warning(const std::string& msg){
    __Logger().log(Logging::Level::WARNING, _info ,msg);
}

void Bind::error(const std::string& msg){
    __Logger().log(Logging::Level::ERROR, _info ,msg);
}

void Bind::critical(const std::string& msg){
    __Logger().log(Logging::Level::CRITICAL, _info ,msg);
}
