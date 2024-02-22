#include "Engine/Logger.hpp"

using namespace Engine;

Logger::Logger() noexcept{}
Logger::~Logger(){}

bool Logger::isShown(Level set) noexcept{
    return static_cast<int>(currlevel) <= static_cast<int>(set);
}

using Level=Logger::Level;

BindedLogger::BindedLogger(
    const std::string& type, const std::string& name
) noexcept: target{type, name}{}

BindedLogger::BindedLogger(
    const std::string& type, int id
) noexcept: target{type, std::to_string(id)}{}

void BindedLogger::debug(const char msg[]) noexcept{
    Logger::get()->log(Level::DEBUG, target.type, target.name, msg);
}

void BindedLogger::info(const char msg[]) noexcept{
    Logger::get()->log(Level::INFO, target.type, target.name, msg);
}

void BindedLogger::warning(const char msg[]) noexcept{
    Logger::get()->log(Level::WARNING, target.type, target.name, msg);
}

void BindedLogger::error(const char msg[]) noexcept{
    Logger::get()->log(Level::ERROR, target.type, target.name, msg);
}

void BindedLogger::critical(const char msg[]) noexcept{
    Logger::get()->log(Level::CRITICAL, target.type, target.name, msg);
}
