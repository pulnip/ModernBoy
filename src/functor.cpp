#include "functor.hpp"

std::shared_ptr<DoNothing> doNothing=std::make_shared<DoNothing>();
std::shared_ptr<AlwaysTrue> alwaysTrue=std::make_shared<AlwaysTrue>();