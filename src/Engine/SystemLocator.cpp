#include <cassert>
#include "Engine/SystemLocator.hpp"

template<typename Service> Service& getService(){
    assert(Locator<Service>::get != nullptr);
    return *Locator<Service>::get();
}
template<typename Service> bool isNull(){
    return Locator<Service>::get() == Service::null;
}

namespace Getter{
    Actors& actors(){ return getService<Actors>(); }
    Logger& logger(){ return getService<Logger>(); }
    InputReceiver& input(){ return getService<InputReceiver>(); }
    Graphics& graphics(){ return getService<Graphics>(); }
    Core& core(){ return getService<Core>(); }
}

#define DECLARE_NULL(Base, Null) \
    std::shared_ptr<Base> Base::null = std::make_shared<Null>()

DECLARE_NULL(Core, NullCore);
DECLARE_NULL(Actors, NullActors);
DECLARE_NULL(Logger, NullLogger);
DECLARE_NULL(InputReceiver, NullInput);
DECLARE_NULL(Graphics, NullGraphics);

#undef DECLARE_NULL

using namespace SystemLocator;

bool SystemLocator::isTest(){
    return isNull<Actors>() or
        isNull<Logger>() or
        isNull<InputReceiver>() or
        isNull<Graphics>() or
        isNull<Core>();
}
