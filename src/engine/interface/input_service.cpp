#include "engine/interface/input_service.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Interface;
using namespace ModernBoy::Input;

void InputService::write(const State& state){
    this->state = state;
}

const State& InputService::snapshot() const{
    return state;
}
