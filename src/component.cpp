#include "component.hpp"

using namespace ModernBoy;

bool ModernBoy::subset(ArchetypeBit a, ArchetypeBit b){
    return (a & b) == a;
}