#ifndef MODERNBOY_GAME_FWD_HPP
#define MODERNBOY_GAME_FWD_HPP

#include <cstdint>
#include "core/alias.hpp"

namespace ModernBoy::Game
{
    using EntityID = ::ModernBoy::ID;
    using ArchetypeBit = uint64_t;

    class Context;
    class EntityRegistry;

    struct Action;
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_FWD_HPP