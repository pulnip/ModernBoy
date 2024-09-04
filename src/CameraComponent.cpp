#include <print>
#include "Actor.hpp"
#include "CameraComponent.hpp"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

CameraComponent::CameraComponent(Actor& actor)
: Component(actor){}

Matrix CameraComponent::view() const noexcept{
    Vector3 fwdDir=XMVector3Rotate(Vector3::UnitZ, owner.transform.quaternion);
    Vector3 upDir=XMVector3Rotate(Vector3::UnitY, owner.transform.quaternion);

    return XMMatrixLookToLH(owner.transform.position, fwdDir, upDir);
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
    owner.transform.position=pos;
}
// void CameraComponent::setEyeDir(const Vector3& dir) noexcept{
//     [[likely]] if(eye.dir!=dir){
//         eye.dir=dir;
//         _view.reset();
//     }
// }
// void CameraComponent::setUpDir(const Vector3& dir) noexcept{
//     [[likely]] if(upDir!=dir){
//         upDir=dir;
//         _view.reset();
//     }
// }
void CameraComponent::setScreenSize(const ipoint2& size) noexcept{
    [[likely]] if(screenSize.x!=size.x){
        screenSize.x=size.x;
        _projection.reset();
    }
    [[likely]] if(screenSize.y!=size.y){
        screenSize.y=size.y;
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
