#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <directxtk/SimpleMath.h>
#include "Component.hpp"
#include "fwd.hpp"
#include "Math.hpp"

namespace ModernBoy{
    class CameraComponent: public Component{
      public:
        ipoint2 screenSize;
        float zNear=0.01f, zFar=100.0f;

        std::optional<float> fieldOfView;

        mutable std::mutex mtx;
        mutable std::optional<DirectX::SimpleMath::Matrix> _projection;

      public:
        CameraComponent(Actor& actor);

        DirectX::SimpleMath::Matrix view() const noexcept;
        DirectX::SimpleMath::Matrix projection() const noexcept;

        bool isPerspective() const noexcept;

        float getAspectRatio() const noexcept;

        void setEyePos(const DirectX::SimpleMath::Vector3& pos) noexcept;
        // void setEyeDir(const DirectX::SimpleMath::Vector3& dir) noexcept;
        // void setUpDir(const DirectX::SimpleMath::Vector3& dir) noexcept;
        void setScreenSize(int width, int height) noexcept;
        void setZ(float _near, float _far) noexcept;
        void setPerspective(float fov) noexcept;
        void setOrthographic() noexcept;
    };
}
