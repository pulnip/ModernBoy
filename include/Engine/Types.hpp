#pragma once

namespace Game{
    using Time=double;

    constexpr int UPDATE_PER_SECOND=60;

    enum class Status{
        FORCE_QUIT,
        GAME_OVER,
        UNEXPECTED
    };
}