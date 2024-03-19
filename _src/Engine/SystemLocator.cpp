#include <cassert>
#include "Engine/SystemLocator.hpp"

Actors& __Actors(){
    assert(Locator<Actors>::get()!=nullptr);
    return *Locator<Actors>::get();
}

Logger& __Logger(){
    assert(Locator<Logger>::get()!=nullptr);
    return *Locator<Logger>::get();
}

using namespace SystemLocator;

bool SystemLocator::isTest(){
    return Locator<Actors>::get() == Actors::null or
        Locator<Logger>::get() == Logger::null; 
}