#include <iostream>
#include "System/Logging/Logger.hpp"

using namespace Logging;

std::shared_ptr<Logger> Logger::null=std::make_shared<NullLogger>();
