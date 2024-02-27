#include "Engine/Logger.hpp"

using namespace Logger;

void Logger::log(Level level, const char msg[]) noexcept{
    if(Impl::get() != nullptr){
        Impl::get()->log(level, msg);
    }
}
void Logger::log(Level level, const int& msg) noexcept{
    if(Impl::get() != nullptr){
        Impl::get()->log(level, msg);
    }
}
void Logger::log(Level level, const double& msg) noexcept{
    if(Impl::get() != nullptr){
        Impl::get()->log(level, msg);
    }
}

void Logger::log(Level level,
    const std::string& type, const std::string& data,
    const char msg[]
) noexcept{
    if(Impl::get() != nullptr){
        Impl::get()->log(level, type, data, msg);
    }
}
void Logger::log(Level level,
    const std::string& type, const int& data,
    const char msg[]
) noexcept{
    if(Impl::get() != nullptr){
        Impl::get()->log(level, type, data, msg);
    }
}
void Logger::log(Level level,
    const std::string& type, const double& data,
    const char msg[]
) noexcept{
    if(Impl::get() != nullptr){
        Impl::get()->log(level, type, data, msg);
    }
}

bool Impl::isShown(Level set) noexcept{
    return static_cast<int>(currlevel) <= static_cast<int>(set);
}

Binded::Binded(
    const std::string& type, const std::string& name
) noexcept: target{type, name}{
    // debug("constructed");
}

Binded::Binded(
    const std::string& type, int id
) noexcept: target{type, std::to_string(id)}{
    // debug("constructed");
}

Binded::~Binded(){
    // debug("destructed");
}

void Binded::debug(const char msg[]) noexcept{
    log(Level::DEBUG, target.type, target.name, msg);
}

void Binded::info(const char msg[]) noexcept{
    log(Level::INFO, target.type, target.name, msg);
}

void Binded::warning(const char msg[]) noexcept{
    log(Level::WARNING, target.type, target.name, msg);
}

void Binded::error(const char msg[]) noexcept{
    log(Level::ERROR, target.type, target.name, msg);
}

void Binded::critical(const char msg[]) noexcept{
    log(Level::CRITICAL, target.type, target.name, msg);
}
