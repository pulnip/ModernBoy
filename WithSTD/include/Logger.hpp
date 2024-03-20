#pragma once

#include "System/Logging/Logger.hpp"

namespace WithSTD{
    class Logger final: public ::Logger{
      public:
        void log(Level, const Info&, const std::string&) override;
        void log(Level, const Info&, const String&) override;
        void log(Level, const Info&, const Integer&) override;
        void log(Level, const Info&, const Double&) override;

        void setCurrLevel(Level level) override{
            currLevel=level;
        }
        Level getCurrLevel() override{
            return currLevel;
        }
        bool isShown(Level set) noexcept override{
            return static_cast<int>(currLevel) <= static_cast<int>(set);
        }

      private:
        Level currLevel;
    };
}