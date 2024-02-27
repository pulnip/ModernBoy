#pragma once

#include "Engine/Logger.hpp"
#include "WithSDL/fwd.hpp"

namespace WithSDL{
    class Logger: public ::Logger::Impl{
      private:
        void log(::Logger::Level, const char msg[]) noexcept override final;
        void log(::Logger::Level, const int& msg) noexcept override final;
        void log(::Logger::Level, const double& msg) noexcept override final;

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