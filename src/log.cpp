#include <SDL3/SDL_log.h>
#include "log.hpp"

using namespace ModernBoy;

static SDL_LogPriority convertToSDL(LogLevel level){
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
    case LogLevel::None:
        return SDL_LOG_PRIORITY_COUNT;
    }
    return SDL_LOG_PRIORITY_INFO;
}
static int convertToInt(LogCategory category){
    switch(category){
        case LogCategory::Application: return 0;
        case LogCategory::Render:      return 1;
        case LogCategory::Input:       return 2;
        case LogCategory::Script:      return 3;
        case LogCategory::Game:        return 4;
        case LogCategory::Network:     return 5;
        case LogCategory::UI:          return 6;
    }
}
static int convertToSDL(LogCategory category){
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
void Logger::log(LogLevel level, LogCategory category, std::string message){
    auto sdlLevel = convertToSDL(level);
    auto sdlCategory = convertToSDL(category);

    LogLevel minLevel = levels[convertToInt(category)];
    if(level < minLevel)
        return;

    static constexpr const char* prefix[] = {
        "   [App]: ", "[Render]: ", " [Input]: ", "[Script]: ",
        "  [Game]: ", "   [Net]: ", "    [UI]: "
    };

    SDL_LogMessage(sdlCategory, sdlLevel, "%s %s", prefix[convertToInt(category)], message.c_str());
}

void Logger::setCategoryLevel(LogCategory category, LogLevel level){
    levels[convertToInt(category)] = level;
}
