#pragma once

#include <exception>
#include <format>
#include <source_location>
#include <string>

namespace ModernBoy{
    class Exception: public std::exception{
        std::string msg;

      public:
        Exception(const std::string& msg="Default Exception",
            const std::source_location& sl=std::source_location::current())
        : msg(std::format("{} ({} {}:{})", msg, 
            sl.file_name(), sl.line(), sl.column()
        )){}

        const char* what() const noexcept override{
            return msg.c_str();
        }
    };

    class NotImplemented: public Exception{
      public:
        NotImplemented(
            const std::source_location& sl=std::source_location::current())
        : Exception("Not Implemented", sl){}
    };
}
