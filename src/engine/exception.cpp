#include <format>
#include "engine/exception.hpp"

void throwIf(bool pred, const std::string& msg,
    const std::source_location& sl)
{
    if(pred) throw SmolException(std::format("{}: ({} {}:{})",
        msg, sl.file_name(), sl.line(), sl.column()
    ));
}