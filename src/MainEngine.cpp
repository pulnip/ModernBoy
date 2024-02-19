#include "MainEngine.hpp"

#include "SubEngine/Logger.hpp"

MainEngine::MainEngine() noexcept
:logger(std::make_shared<WithSTD::SubEngine::Logger_default>()){}