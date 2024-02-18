#include <iostream>
#include <map>

#include "SubEngine/Logger.hpp"

using namespace Game::SubEngine;

void Logger::setLevel(const LogLevel& level) noexcept{
    std::map<LogLevel, const std::string> levelMap={
        {LogLevel::DEBUG,    "DEBUG"},
        {LogLevel::INFO,     "INFO"},
        {LogLevel::WARNING,  "WARNING"},
        {LogLevel::ERROR,    "ERROR"},
        {LogLevel::CRITICAL, "CRITICAL"},
    };

    this->level=levelMap[level];
}

using namespace WithSTD::SubEngine;

void Logger_default::log(const std::string& msg) noexcept{
    std::cout<<level<<": "<<msg<<'\n';
}

void Logger_default::log(
    const std::string& type, const std::string& data,
    const std::string& state
) noexcept{
    std::cout<<level<<": "<<type<<'('<<data<<") "<<state<<'\n';
}

void Logger_default::log(
    const std::string& type, const int& data,
    const std::string& state
) noexcept{
    std::cout<<level<<": "<<type<<'('<<data<<") "<<state<<'\n';
}

void Logger_default::log(
    const std::string& type, const double& data,
    const std::string& state
) noexcept{
    std::cout<<level<<": "<<type<<'('<<data<<") "<<state<<'\n';
}
