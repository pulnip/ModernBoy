#ifndef MODERNBOY_LOG_HPP
#define MODERNBOY_LOG_HPP

#include <string>
#include <SDL3/SDL_log.h>

namespace ModernBoy
{
    enum class LogLevel{
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Critical
    };
    enum class LogCategory{
        Application = SDL_LOG_CATEGORY_APPLICATION,
        Render      = SDL_LOG_CATEGORY_RENDER,
        Input       = SDL_LOG_CATEGORY_INPUT,
        Script      = SDL_LOG_CATEGORY_CUSTOM,
        Game        = SDL_LOG_CATEGORY_CUSTOM + 1,
        Network     = SDL_LOG_CATEGORY_CUSTOM + 2,
        UI          = SDL_LOG_CATEGORY_CUSTOM + 3,
    };

    class Logger{
    public:
        Logger() = default;
        ~Logger() = default;

        void log(LogLevel level, LogCategory category, const std::string& message);
        void setCategoryLevel(LogCategory category, LogLevel level);

        LogLevel application;
        LogLevel render;
        LogLevel input;
        LogLevel script;
        LogLevel game;
        LogLevel network;
        LogLevel ui;
    };

    inline Logger logger;

    inline void LogTrace(LogCategory category, const std::string& msg){
        logger.log(LogLevel::Debug, category, msg);
    }
    inline void LogDebug(LogCategory category, const std::string& msg){
        logger.log(LogLevel::Debug, category, msg);
    }
    inline void LogInfo(LogCategory category, const std::string& msg){
        logger.log(LogLevel::Info, category, msg);
    }
    inline void LogWarn(LogCategory category, const std::string& msg){
        logger.log(LogLevel::Warn, category, msg);
    }
    inline void LogError(LogCategory category, const std::string& msg){
        logger.log(LogLevel::Error, category, msg);
    }
    inline void LogCritical(LogCategory category, const std::string& msg){
        logger.log(LogLevel::Critical, category, msg);
    }
} // namespace ModernBoy


#endif // MODERNBOY_LOG_HPP