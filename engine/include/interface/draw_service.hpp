#ifndef MODERNBOY_INTERFACE_DRAWSERVICE_HPP
#define MODERNBOY_INTERFACE_DRAWSERVICE_HPP

#include <mutex>
#include <vector>
#include "math/type.hpp"
#include "fwd.hpp"

namespace ModernBoy::Interface
{
    struct MeshDrawCall{
        DEFINE_TRANSFORM;
        float alpha;
        SubmeshHandle submeshHandle;
        MaterialHandle materialHandle;
        ShaderHandle shaderHandle;
        EntityID entity;
    };

    class DrawService{
    public:
        DrawService() = default;
        ~DrawService() = default;

        void write(const Line&);
        void write(const Sphere&);
        void write(const MeshDrawCall&);

        std::vector<Line> drainLines();
        std::vector<Sphere> drainSpheres();
        std::vector<MeshDrawCall> drainDrawCalls();

    private:
        void sortDrawCalls();

        std::vector<Line> lines;
        std::vector<Sphere> spheres;
        std::vector<MeshDrawCall> drawCalls;

        std::mutex lineMtx, sphereMtx, drawCallMtx;
    };
}

#endif // MODERNBOY_INTERFACE_DRAWSERVICE_HPP
