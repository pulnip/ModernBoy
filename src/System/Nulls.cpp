#include "System/Managing/Actors.hpp"
#include "System/Logging/Logger.hpp"
#include "System/Input/InputReceiver.hpp"

#define DECLARE_NULL(Base, Null) \
    std::shared_ptr<Base> Base::null = std::make_shared<Null>()

DECLARE_NULL(Actors, NullActors);
DECLARE_NULL(Logger, NullLogger);
DECLARE_NULL(InputReceiver, NullInput);

#undef DECLARE_NULL