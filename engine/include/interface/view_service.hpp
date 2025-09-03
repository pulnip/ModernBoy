#ifndef MODERNBOY_INTERFACE_VIEWSERVICE_HPP
#define MODERNBOY_INTERFACE_VIEWSERVICE_HPP

#include <mutex>
#include <vector>
#include "math/type.hpp"

namespace ModernBoy::Interface
{
    struct CameraObject{
        DEFINE_TRANSFORM;

        CameraType type;
        float fov;
        float nearPlane;
        float farPlane;
        Projection projection;
    };

    class ViewService{
    public:
        ViewService() = default;
        ~ViewService() = default;

        void write(const CameraObject&);

        std::vector<CameraObject> drainCameraObjects();

    private:
        std::vector<CameraObject> cameraObjects;

        std::mutex mtx;
    };
}

#endif // MODERNBOY_INTERFACE_VIEWSERVICE_HPP
