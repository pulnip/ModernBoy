#ifndef MODERNBOY_RENDER_SYSTEM_HPP
#define MODERNBOY_RENDER_SYSTEM_HPP

#include <chrono>
#include <map>
#include <thread>
#include <unordered_set>
#include <SDL3/SDL_video.h>
#include "fwd.hpp"
#include "common/alias.hpp"
#include "render/renderer.hpp"

namespace ModernBoy::Render
{
    class System{
    public:
        System(AppState& app, SDL_Window* window);
        ~System() = default;

        void update(std::stop_token stoken);

    private:
        AppState& app;

        // MUST CONSTRUCT queue FIRST
    public:
        Renderer renderer;
    private:
        std::jthread commandThread;
        // MUST DESTRUCT stop_souce FIRST
        std::stop_source stsrc;
    };
} // namespace ModernBoy::Render

#endif // MODERNBOY_RENDER_SYSTEM_HPP