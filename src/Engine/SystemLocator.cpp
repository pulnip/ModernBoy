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
}

using namespace SystemLocator;

bool SystemLocator::isTest(){
    return isNull<Actors>() or
        isNull<Logger>() or
        isNull<InputReceiver>() or
        isNull<Graphics>();
}
