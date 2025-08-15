#ifndef MODERNBOY_EXCEPTION_HPP
#define MODERNBOY_EXCEPTION_HPP

#include <exception>
#include <source_location>
#include <string>

class SmolException: public std::exception{
    std::string msg;

public:
    SmolException(const std::string& message)
    :msg(message){}

    const char* what() const noexcept override{ return msg.c_str(); }
};

extern void throwIf(bool pred, const std::string& msg = "(NO MESSAGE)",
    const std::source_location& sl=std::source_location::current());

#endif // MODERNBOY_EXCEPTION_HPP