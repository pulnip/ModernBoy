#include "Engine/Main.hpp"
#include "Engine/SystemLocator.hpp"

// test
#include "System/Logging/Logger.hpp"

template<typename T>
using provide=Locator<T>::provide;

Main::Main() noexcept{
    provide<Logger>(std::make_shared<NullLogger>());
    provide<Actors>(std::make_shared<Actors>());

}

void Main::start() noexcept{
    // Core::get()->run();
}