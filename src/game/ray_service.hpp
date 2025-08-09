#ifndef MODERNBOY_GAME_RAYSERVICE_HPP
#define MODERNBOY_GAME_RAYSERVICE_HPP

#include <mutex>
#include <vector>
#include "core/math/type.hpp"

namespace ModernBoy::Game
{
    class RayService{
    public:
        RayService() = default;
        ~RayService() = default;

        void write(const Ray&);

        std::vector<Ray> drainRays();

    private:
        std::vector<Ray> rays;

        std::mutex mtx;
    };
}

#endif // MODERNBOY_GAME_RAYSERVICE_HPP
