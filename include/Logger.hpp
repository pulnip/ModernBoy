#pragma once

#include <string>

enum class LogLevel{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

namespace Game{
    namespace Plugin{
        class Logger{
        public:
            virtual void log(const std::string& msg) noexcept=0;

            virtual void log( const std::string& type, const std::string& data,
                const std::string& state
            ) noexcept=0;
            virtual void log( const std::string& type, const int& data,
                const std::string& state
            ) noexcept=0;
            virtual void log( const std::string& type, const double& data,
                const std::string& state
            ) noexcept=0;

            void setLevel(const LogLevel& _level) noexcept;

        protected:
            std::string level="DEFAULT";
        };
    }
}

namespace WithSTD{
    namespace Plugin{
        class Logger_default final: public Game::Plugin::Logger{
        private:
            void log(const std::string& msg) noexcept override final;

            void log(const std::string& type, const std::string& data,
                const std::string& state
            ) noexcept override final;
            void log(const std::string& type, const int& data,
                const std::string& state
            ) noexcept override final;
            void log(const std::string& type, const double& data,
                const std::string& state
            ) noexcept override final;
        };
    }
}