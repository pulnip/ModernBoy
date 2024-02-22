#pragma once

#include "Engine/Logger.hpp"
#include "WithSDL/fwd.hpp"

namespace WithSDL{
    class Logger: public Engine::Logger{
      private:
        void log(Level, const char msg[]) noexcept override final;
        void log(Level, const int& msg) noexcept override final;
        void log(Level, const double& msg) noexcept override final;

        void log(Level,
            const std::string& type, const std::string& data,
            const char msg[]
        ) noexcept override final;
        void log(Level,
            const std::string& type, const int& data,
            const char msg[]
        ) noexcept override final;
        void log(Level,
            const std::string& type, const double& data,
            const char msg[]
        ) noexcept override final;
    };
}