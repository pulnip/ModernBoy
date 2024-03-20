#pragma once

#include "System/Logging/Logger.hpp"

namespace Logging{
    class Bind{
      public:
        Bind(const std::string& type, const std::string& name) noexcept:
        _info{type, name}{};
    
        void log(const std::string& msg);
        void log(const String& data);
        void log(const Integer& data);
        void log(const Double& data);

        void debug(const std::string& msg);
        void info(const std::string& msg);
        void warning(const std::string& msg);
        void error(const std::string& msg);
        void critical(const std::string& msg);

      private:
        Info _info;
    };
}
