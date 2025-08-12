#ifndef MODERNBOY_INTERFACE_DRAWSERVICE_HPP
#define MODERNBOY_INTERFACE_DRAWSERVICE_HPP

#include <mutex>
#include <vector>
#include "core/math/type.hpp"
#include "engine/fwd.hpp"

namespace ModernBoy::Interface
{
    struct MeshObject{
        DEFINE_TRANSFORM;
        float alpha;
        MeshHandle meshHandle;
        TextureHandle texHandle;
        ShaderHandle shaderHandle;
        EntityID entity;
    };

    class DrawService{
    public:
        DrawService() = default;
        ~DrawService() = default;

        void write(const Line&);
        void write(const Sphere&);
        void write(const MeshObject&);

        std::vector<Line> drainLines();
        std::vector<Sphere> drainSpheres();
        std::vector<MeshObject> drainMeshObjects();

    private:
        void sortMeshObjects();

        std::vector<Line> lines;
        std::vector<Sphere> spheres;
        std::vector<MeshObject> meshObjects;

        std::mutex lineMtx, sphereMtx, meshMtx;
    };
}

#endif // MODERNBOY_INTERFACE_DRAWSERVICE_HPP
