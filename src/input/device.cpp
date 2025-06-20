#include <utility>
#include "input/device.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Input;

Device::Device(){}
Device::~Device(){}
Device::Device(Device&& other){
    moveFrom(std::move(other)); }
Device& Device::operator=(Device&& other){
    moveFrom(std::move(other)); }
void Device::moveFrom(Device&& other){}

State Device::fetch(){
    State state;

    return state;
}
