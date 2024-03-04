#pragma once

#include <string>
#include "TinyTraits.hpp"
#include "Blueprint.hpp"
#include "myfwd.hpp"

namespace Logger{
    enum class Level{
        DEBUG=0,
        INFO=1,
        WARNING=2,
        ERROR=3,
        CRITICAL=4
    };

    void log(Level, const char msg[]) noexcept;
    void log(Level, const int& msg) noexcept;
    void log(Level, const double& msg) noexcept;

    void log(Level,
        const std::string& type, const std::string& data,
        const char msg[]
    ) noexcept;
    void log(Level,
        const std::string& type, const int& data,
        const char msg[]
    ) noexcept;
    void log(Level,
        const std::string& type, const double& data,
        const char msg[]
    ) noexcept;

    class Impl: public MakableSingleton<Impl>{
        friend class ::MainEngine;
      public:
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
        Level currlevel=Level::DEBUG;
    };

    class Binded{
      public:
        Binded(
            const std::string& type, const std::string& name
        ) noexcept;
        Binded(
            const std::string& type, int id
        ) noexcept;
        virtual ~Binded();

        void debug(const char msg[]) noexcept;
        void info(const char msg[]) noexcept;
        void warning(const char msg[]) noexcept;
        void error(const char msg[]) noexcept;
        void critical(const char msg[]) noexcept;

      private:
        Blueprint::LoggerInfo target;
    };
}

namespace WithSTD{
    class Logger: public ::Logger::Impl{
      public:
        static void preConstruct() noexcept{}
        void postConstruct() noexcept override final{}
        static void make() noexcept{
            ::Logger::Impl::make<Logger>();
        }

      private:
        void log(::Logger::Level,
            const char msg[]
        ) noexcept override final;
        void log(::Logger::Level,
            const int& msg
        ) noexcept override final;
        void log(::Logger::Level,
            const double& msg
        ) noexcept override final;

        void log(::Logger::Level,
            const std::string& type, const std::string& data,
            const char msg[]
        ) noexcept override final;
        void log(::Logger::Level,
            const std::string& type, const int& data,
            const char msg[]
        ) noexcept override final;
        void log(::Logger::Level,
            const std::string& type, const double& data,
            const char msg[]
        ) noexcept override final;
    };
}