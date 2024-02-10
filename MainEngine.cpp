#include "MainEngine.hpp"

#include "Logger.hpp"

MainEngine::MainEngine() noexcept
:logger(std::make_shared<Logger_default>()){}