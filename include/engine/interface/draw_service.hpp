#ifndef MODERNBOY_INTERFACE_DRAWSERVICE_HPP
#define MODERNBOY_INTERFACE_DRAWSERVICE_HPP

#include <mutex>
#include <vector>
#include "core/math/type.hpp"

namespace ModernBoy::Interface
{
    class DrawService{
    public:
        DrawService() = default;
        ~DrawService() = default;

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

#endif // MODERNBOY_INTERFACE_DRAWSERVICE_HPP
