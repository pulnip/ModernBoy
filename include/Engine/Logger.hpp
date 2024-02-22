#pragma once

#include <string>
#include "TinyTraits.hpp"
#include "Blueprint.hpp"
#include "myfwd.hpp"

namespace Engine{
    class Logger: public Singleton<Logger>{
        friend class ::MainEngine;
      public:
        enum class Level{
            DEBUG=0,
            INFO=1,
            WARNING=2,
            ERROR=3,
            CRITICAL=4
        };

      public:
        Logger() noexcept;
        virtual ~Logger();

        virtual void log(Level, const char msg[]) noexcept=0;
        virtual void log(Level, const int& msg) noexcept=0;
        virtual void log(Level, const double& msg) noexcept=0;

        virtual void log(Level,
            const std::string& type, const std::string& data,
            const char msg[]
        ) noexcept=0;
        virtual void log(Level,
            const std::string& type, const int& data,
            const char msg[]
        ) noexcept=0;
        virtual void log(Level,
            const std::string& type, const double& data,
            const char msg[]
        ) noexcept=0;

      protected:
        bool isShown(Level) noexcept;

      protected:
        Level currlevel;
    };

    struct BindedLogger{
        BindedLogger(
            const std::string& type, const std::string& name
        ) noexcept;
        BindedLogger(
            const std::string& type, int id
        ) noexcept;

        void debug(const char msg[]) noexcept;
        void info(const char msg[]) noexcept;
        void warning(const char msg[]) noexcept;
        void error(const char msg[]) noexcept;
        void critical(const char msg[]) noexcept;

        Blueprint::LoggerInfo target;
    };
}