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
        ~System();

    private:
        void update(std::stop_token stoken);

    public:
        Renderer renderer;

    private:
        AppState& app;

        std::stop_source stsrc;
        std::jthread commandThread;
    };
} // namespace ModernBoy::Render

#endif // MODERNBOY_RENDER_SYSTEM_HPP