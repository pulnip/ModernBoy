#include "game/context.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

size_t Context::yield_count() const noexcept{
    return 0;
}

Generator<void> Context::updateTask(DeltaTime){
    co_return;
}

Generator<void> Context::update(DeltaTime){
    co_return;
}
