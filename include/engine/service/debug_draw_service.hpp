#ifndef MODERNBOY_SERVICE_DEBUGDRAWSERVICE_HPP
#define MODERNBOY_SERVICE_DEBUGDRAWSERVICE_HPP

#include <mutex>
#include <vector>
#include "core/math/type.hpp"

namespace ModernBoy::Service
{
    class DebugDrawService{
    public:
        DebugDrawService() = default;
        ~DebugDrawService() = default;

        void write(const Line&);
        void write(const Sphere&);

        std::vector<Line> drainLines();
        std::vector<Sphere> drainSpheres();

    private:
        std::vector<Line> lines;
        std::vector<Sphere> spheres;

        std::mutex lineMtx, sphereMtx;
    };
}

#endif // MODERNBOY_SERVICE_DEBUGDRAWSERVICE_HPP
