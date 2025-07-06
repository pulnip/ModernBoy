#include <SDL3/SDL_log.h>
#include "log.hpp"

using namespace ModernBoy;

static SDL_LogPriority convertLevel(LogLevel level){
    switch (level){
    case LogLevel::Trace:
        return SDL_LOG_PRIORITY_VERBOSE;
    case LogLevel::Debug:
        return SDL_LOG_PRIORITY_DEBUG;
    case LogLevel::Info:
        return SDL_LOG_PRIORITY_INFO;
    case LogLevel::Warn:
        return SDL_LOG_PRIORITY_WARN;
    case LogLevel::Error:
        return SDL_LOG_PRIORITY_ERROR;
    case LogLevel::Critical:
        return SDL_LOG_PRIORITY_CRITICAL;
    }
    return SDL_LOG_PRIORITY_INFO;
}
static int convertCategory(LogCategory category){
    switch(category){
    case LogCategory::Application:
        return SDL_LOG_CATEGORY_APPLICATION;
    case LogCategory::Render:
        return SDL_LOG_CATEGORY_RENDER;
    case LogCategory::Input:
        return SDL_LOG_CATEGORY_INPUT;
    case LogCategory::Script:
        return SDL_LOG_CATEGORY_CUSTOM;
    case LogCategory::Game:
        return SDL_LOG_CATEGORY_CUSTOM + 1;
    case LogCategory::Network:
        return SDL_LOG_CATEGORY_CUSTOM + 2;
    case LogCategory::UI:
        return SDL_LOG_CATEGORY_CUSTOM + 3;
    default:
        return SDL_LOG_CATEGORY_APPLICATION;
    }
}
void Logger::log(LogLevel level, LogCategory category, const std::string& message){
    auto sdlLevel = convertLevel(level);
    auto sdlCategory = convertCategory(category);
    LogLevel minLevel = LogLevel::Info;
    switch(category){
        case LogCategory::Application: minLevel = application; break;
        case LogCategory::Render:      minLevel = render;      break;
        case LogCategory::Input:       minLevel = input;       break;
        case LogCategory::Script:      minLevel = script;      break;
        case LogCategory::Game:        minLevel = game;        break;
        case LogCategory::Network:     minLevel = network;     break;
        case LogCategory::UI:          minLevel = ui;          break;
    }
    if(level < minLevel)
        return;
    SDL_LogMessage(sdlCategory, sdlLevel, "%s", message.c_str());
}

void Logger::setCategoryLevel(LogCategory category, LogLevel level){
    switch(category){
    case LogCategory::Application:
        application = level;
        break;
    case LogCategory::Render:
        render = level;
        break;
    case LogCategory::Input:
        input = level;
        break;
    case LogCategory::Script:
        script = level;
        break;
    case LogCategory::Game:
        game = level;
        break;
    case LogCategory::Network:
        network = level;
        break;
    case LogCategory::UI:
        ui = level;
        break;
    }
}
