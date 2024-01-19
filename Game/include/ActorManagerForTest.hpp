#pragma once

#include "SubEngine/ActorManager.hpp"

class ActorManagerForTest: public ActorManager{
private:
    void injectDependency() noexcept override final;
};