#pragma once

#include <string>

#include "SubEngine/SubEngine.hpp"

enum class LogLevel{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

namespace Game{
    namespace SubEngine{
        class Logger: public Interface{
        public:
            virtual void log(const std::string& msg) noexcept=0;

            virtual void log(
                const std::string& type, const std::string& data,
                const std::string& state
            ) noexcept=0;
            virtual void log(
                const std::string& type, const int& data,
                const std::string& state
            ) noexcept=0;
            virtual void log(
                const std::string& type, const double& data,
                const std::string& state
            ) noexcept=0;

            Type getType() const noexcept override final{
                return Type::Logger;
            }
            void setLevel(const LogLevel& _level) noexcept;

        private:
            void update(const Time&) noexcept override final{}

        protected:
            std::string level="DEFAULT";
        };
    }
}

namespace WithSTD{
    namespace SubEngine{
        class Logger_default final: public Game::SubEngine::Logger{
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