#ifndef MODERNBOY_LOG_HPP
#define MODERNBOY_LOG_HPP

#include <string>
#include <SDL3/SDL_log.h>

#include <format>

namespace ModernBoy
{
#ifndef DISABLE_LOG
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
        App         = SDL_LOG_CATEGORY_APPLICATION,
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
        Logger();
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

    #define DECL_LOG_FUNC(level) template<typename...Args> \
    inline void Log##level(LogCategory category, \
        std::format_string<Args...> msg, Args&&... args \
    ){ \
        logger.log(LogLevel::level, category, std::move(msg), \
        std::forward(args)...); \
    }
    DECL_LOG_FUNC(Trace)
    DECL_LOG_FUNC(Debug)
    DECL_LOG_FUNC(Info)
    DECL_LOG_FUNC(Warn)
    DECL_LOG_FUNC(Error)
    DECL_LOG_FUNC(Critical)

    #define DECL_DOMAIN_LEVEL_LOG_FUNC(category, level) template<typename... Args> \
    inline void category##level(std::format_string<Args...> msg, Args&&... args){ \
        logger.log(LogLevel::level, LogCategory::category, std::move(msg), \
        std::forward<Args>(args)...); \
    }

    #define DECL_DOMAIN_LOG_FUNC(category) \
        DECL_DOMAIN_LEVEL_LOG_FUNC(category, Trace) \
        DECL_DOMAIN_LEVEL_LOG_FUNC(category, Debug) \
        DECL_DOMAIN_LEVEL_LOG_FUNC(category, Info) \
        DECL_DOMAIN_LEVEL_LOG_FUNC(category, Warn) \
        DECL_DOMAIN_LEVEL_LOG_FUNC(category, Error) \
        DECL_DOMAIN_LEVEL_LOG_FUNC(category, Critical)
    DECL_DOMAIN_LOG_FUNC(App)
    DECL_DOMAIN_LOG_FUNC(Render)
    DECL_DOMAIN_LOG_FUNC(Input)
    DECL_DOMAIN_LOG_FUNC(Script)
    DECL_DOMAIN_LOG_FUNC(Game)
    DECL_DOMAIN_LOG_FUNC(Network)
    DECL_DOMAIN_LOG_FUNC(UI)
#else
    #define AppTrace(...)
    #define AppDebug(...)
    #define AppInfo(...)
    #define AppWarn(...)
    #define AppError(...)
    #define AppCritical(...)
    #define RenderTrace(...)
    #define RenderDebug(...)
    #define RenderInfo(...)
    #define RenderWarn(...)
    #define RenderError(...)
    #define RenderCritical(...)
    #define InputTrace(...)
    #define InputDebug(...)
    #define InputInfo(...)
    #define InputWarn(...)
    #define InputError(...)
    #define InputCritical(...)
    #define ScriptTrace(...)
    #define ScriptDebug(...)
    #define ScriptInfo(...)
    #define ScriptWarn(...)
    #define ScriptError(...)
    #define ScriptCritical(...)
    #define GameTrace(...)
    #define GameDebug(...)
    #define GameInfo(...)
    #define GameWarn(...)
    #define GameError(...)
    #define GameCritical(...)
    #define NetworkTrace(...)
    #define NetworkDebug(...)
    #define NetworkInfo(...)
    #define NetworkWarn(...)
    #define NetworkError(...)
    #define NetworkCritical(...)
    #define UITrace(...)
    #define UIDebug(...)
    #define UIInfo(...)
    #define UIWarn(...)
    #define UIError(...)
    #define UICritical(...)
#endif
} // namespace ModernBoy


#endif // MODERNBOY_LOG_HPP