#include <print>
#include "CameraComponent.hpp"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

CameraComponent::CameraComponent(Actor& actor)
: Component(actor){}

Matrix CameraComponent::view() const noexcept{
    std::lock_guard lg(mtx);

    [[unlikely]] if(!_view.has_value())
        _view=XMMatrixLookToLH(eye.p0, eye.dir, upDir);

    return _view.value();
}

Matrix CameraComponent::projection() const noexcept{
    std::lock_guard lg(mtx);

    [[unlikely]] if(!_projection.has_value()){
        const auto aspectRatio=getAspectRatio();

        [[likely]] if(isPerspective()){
            _projection=XMMatrixPerspectiveFovLH(
                fieldOfView.value(), aspectRatio,
                zNear, zFar
            );
        } else{
            _projection=XMMatrixOrthographicOffCenterLH(
                -aspectRatio, aspectRatio, -1.0f, 1.0f,
                zNear, zFar
            );
        }
    }

    return _projection.value();
}

bool CameraComponent::isPerspective() const noexcept{
    return fieldOfView.has_value();
}
float CameraComponent::getAspectRatio() const noexcept{
    return static_cast<float>(screenSize.x)/screenSize.y;
}

void CameraComponent::setEyePos(const Vector3& pos) noexcept{
    [[likely]] if(eye.p0!=pos){
        eye.p0=pos;
        _view.reset();
    }
}
void CameraComponent::setEyeDir(const Vector3& dir) noexcept{
    [[likely]] if(eye.dir!=dir){
        eye.dir=dir;
        _view.reset();
    }
}
void CameraComponent::setUpDir(const Vector3& dir) noexcept{
    [[likely]] if(upDir!=dir){
        upDir=dir;
        _view.reset();
    }
}
void CameraComponent::setScreenSize(int w, int h) noexcept{
    [[likely]] if(screenSize.x!=w){
        screenSize.x=w;
        _projection.reset();
    }
    [[likely]] if(screenSize.y!=h){
        screenSize.y=h;
        _projection.reset();
    }
}
void CameraComponent::setZ(float n, float f) noexcept{
    [[likely]] if(zNear!=n){
        zNear=n;
        _projection.reset();
    }
    [[likely]] if(zFar!=f){
        zFar=f;
        _projection.reset();
    }
}
void CameraComponent::setPerspective(float fov) noexcept{
    [[likely]] if(!fieldOfView.has_value() || fieldOfView!=fov){
        [[likely]] if(fov!=0.0f){
            fieldOfView=fov;
            _projection.reset();
        }
        else setOrthographic();
    }
}
void CameraComponent::setOrthographic() noexcept{
    [[likely]] if(isPerspective()){
        fieldOfView.reset();
        _projection.reset();
    }
}
