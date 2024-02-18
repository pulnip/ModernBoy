#pragma once

#include <memory>

#include "Makable.hpp"
#include "gefwd.hpp"

class MainEngine: public Makable<MainEngine>{
public:
    Game::Heart getEngine() noexcept{ return gameEngine; }

private:
    Game::Heart gameEngine;
};