#ifndef MODERNBOY_GAME_FWD_HPP
#define MODERNBOY_GAME_FWD_HPP

#include <cstdint>
#include "common/alias.hpp"

namespace ModernBoy::Game
{
    using EntityID = ::ModernBoy::ID;
    using ArchetypeBit = uint64_t;

    class Context;

    struct ActionComponent;
    struct SparseChunk;
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_FWD_HPP