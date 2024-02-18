#pragma once

#include "SubEngine/ActorManager.hpp"

class ActorManagerForTest: public Game::SubEngine::ActorManager{
public:
    ActorManagerForTest() noexcept=default;
    ~ActorManagerForTest()=default;

private:
    void postConstruct() noexcept override final;
};