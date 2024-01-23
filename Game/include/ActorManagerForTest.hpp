#pragma once

#include "SubEngine/ActorManager.hpp"

class ActorManagerForTest: public ActorManager{
public:
    ActorManagerForTest() noexcept=default;
    ~ActorManagerForTest()=default;

private:
    void setAttribute() noexcept override final;
};