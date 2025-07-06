#ifndef MODERNBOY_LOG_HPP
#define MODERNBOY_LOG_HPP

#include <string>
#include <SDL3/SDL_log.h>

#include <format>

namespace ModernBoy
{
    enum class LogLevel{
        Trace       = 0,
        Debug       = 1,
        Info        = 2,
        Warn        = 3,
        Error       = 4,
        Critical    = 5,
        None        = 6
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
    constexpr auto NUM_LOG_CATEGORY = 7;

    class Logger{
    public:
        Logger() = default;
        ~Logger() = default;

        template<typename... Args>
        void log(LogLevel level, LogCategory category,
            std::format_string<Args...>&& msg, Args&&... args
        ){
            log(level, category, std::format(std::move(msg), std::forward<Args>(args)...));
        }

    private:
        void log(LogLevel level, LogCategory category, std::string msg);
        void setCategoryLevel(LogCategory category, LogLevel level);

        LogLevel levels[NUM_LOG_CATEGORY];
    };

    inline Logger logger;

    template<typename... Args>
    inline void LogTrace(LogCategory category, std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Debug, category, std::forward(msg), std::forward(args)...);
    }
    template<typename... Args>
    inline void LogDebug(LogCategory category, std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Debug, category, std::forward(msg), std::forward(args)...);
    }
    template<typename... Args>
    inline void LogInfo(LogCategory category, std::format_string<Args...>&& msg, Args&&... args){
        logger.log(LogLevel::Info, category, std::move(msg), std::forward<Args>(args)...);
    }
    template<typename... Args>
    inline void LogWarn(LogCategory category, std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Warn, category, std::forward(msg), std::forward(args)...);
    }
    template<typename... Args>
    inline void LogError(LogCategory category, std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Error, category, std::forward(msg), std::forward(args)...);
    }
    template<typename... Args>
    inline void LogCritical(LogCategory category, std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Critical, category, std::forward(msg), std::forward(args)...);
    }

    template<typename... Args>
    inline void AppTrace(std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Debug, LogCategory::Application, std::forward(msg), std::forward(args)...);
    }
    template<typename... Args>
    inline void AppDebug(std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Debug, LogCategory::Application, std::forward(msg), std::forward(args)...);
    }
    template<typename... Args>
    inline void AppInfo(std::format_string<Args...>&& msg, Args&&... args){
        logger.log(LogLevel::Info, LogCategory::Application, std::move(msg), std::forward<Args>(args)...);
    }
    template<typename... Args>
    inline void AppWarn(std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Warn, LogCategory::Application, std::forward(msg), std::forward(args)...);
    }
    template<typename... Args>
    inline void AppError(std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Error, LogCategory::Application, std::forward(msg), std::forward(args)...);
    }
    template<typename... Args>
    inline void AppCritical(std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Critical, LogCategory::Application, std::forward(msg), std::forward(args)...);
    }

    template<typename... Args>
    inline void RenderTrace(std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Debug, LogCategory::Render, std::forward(msg), std::forward(args)...);
    }
    template<typename... Args>
    inline void RenderDebug(std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Debug, LogCategory::Render, std::forward(msg), std::forward(args)...);
    }
    template<typename... Args>
    inline void RenderInfo(std::format_string<Args...>&& msg, Args&&... args){
        logger.log(LogLevel::Info, LogCategory::Render, std::move(msg), std::forward<Args>(args)...);
    }
    template<typename... Args>
    inline void RenderWarn(std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Warn, LogCategory::Render, std::forward(msg), std::forward(args)...);
    }
    template<typename... Args>
    inline void RenderError(std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Error, LogCategory::Render, std::forward(msg), std::forward(args)...);
    }
    template<typename... Args>
    inline void RenderCritical(std::format_string<Args...> msg, Args&&... args){
        logger.log(LogLevel::Critical, LogCategory::Render, std::forward(msg), std::forward(args)...);
    }
} // namespace ModernBoy


#endif // MODERNBOY_LOG_HPP