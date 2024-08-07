#include <directxmath/DirectXMath.h>
#include "Camera.hpp"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

Matrix Camera::view() const noexcept{
    std::lock_guard lg(mtx);

    [[unlikely]] if(!_view.has_value())
        _view=XMMatrixLookToLH(eye.p0, eye.dir, upDir);

    return _view.value();
}

Matrix Camera::projection() const noexcept{
    std::lock_guard lg(mtx);

    [[unlikely]] if(!_projection.has_value()){
        [[likely]] if(isPerspective()){
            _projection=XMMatrixPerspectiveFovLH(
                fieldOfView.value(), getAspectRatio(),
                zRange.point0, zRange.point1
            );
        } else{
            _projection=XMMatrixOrthographicOffCenterLH(
                -getAspectRatio(), getAspectRatio(), -1.0f, 1.0f,
                zRange.point0, zRange.point1
            );
        }
    }

    return _projection.value();
}

bool Camera::isPerspective() const noexcept{
    return fieldOfView.has_value();
}
void Camera::setEyePos(const Vector3& pos) noexcept{
    [[likely]] if(eye.p0!=pos){
        eye.p0=pos;
        _view.reset();
    }
}
void Camera::setEyeDir(const Vector3& dir) noexcept{
    [[likely]] if(eye.dir!=dir){
        eye.dir=dir;
        _view.reset();
    }
}
void Camera::setUpDir(const Vector3& dir) noexcept{
    [[likely]] if(upDir!=dir){
        upDir=dir;
        _view.reset();
    }
}
void Camera::setScreenSize(const ipoint2& size) noexcept{
    [[likely]] if(screenSize!=size){
        screenSize=size;
        _projection.reset();
    }
}
void Camera::setNearZ(const float& nearZ) noexcept{
    [[likely]] if(zRange.point0!=nearZ){
        zRange.point0=nearZ;
        _projection.reset();
    }
}
void Camera::setFarZ(const float& farZ) noexcept{
    [[likely]] if(zRange.point1!=farZ){
        zRange.point1=farZ;
        _projection.reset();
    }
}
void Camera::setPerspective(const float& fov) noexcept{
    [[likely]] if(!fieldOfView.has_value() || fieldOfView!=fov){
        [[likely]] if(fov!=0.0f){
            fieldOfView=fov;
            _projection.reset();
        }
        else setOrthographic();
    }
}
void Camera::setOrthographic() noexcept{
    [[likely]] if(isPerspective()){
        fieldOfView.reset();
        _projection.reset();
    }
}
