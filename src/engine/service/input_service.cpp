#include "engine/service/input_service.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Service;
using namespace ModernBoy::Input;

void InputService::write(const State& state){
    this->state = state;
}

const State& InputService::snapshot() const{
    return state;
}
