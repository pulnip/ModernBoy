#pragma once

#include <mutex>
#include <optional>
#include "Ray.hpp"

namespace ModernBoy{
    class Camera{
        using Matrix=DirectX::SimpleMath::Matrix;

        Ray eye={
            .p0=Vector3::Zero,
            .dir=Vector3::UnitZ
        };
        Vector3 upDir=Vector3::UnitY;
        ipoint2 screenSize;
        Line1 zRange{0.01f, 100.0f};

        // Radian relative to upDir
        // nullopt for Orthographic projection
        std::optional<float> fieldOfView=70.0f;

        mutable std::mutex mtx;
        mutable std::optional<Matrix> _view=std::nullopt;
        mutable std::optional<Matrix> _projection=std::nullopt;

      public:
        Matrix view() const noexcept;
        Matrix projection() const noexcept;

        bool isPerspective() const noexcept;
        void setEyePos(const Vector3& pos) noexcept;
        void setEyeDir(const Vector3& dir) noexcept;
        void setUpDir(const Vector3& dir) noexcept;
        void setScreenSize(const ipoint2& screenSize) noexcept;
        void setNearZ(const float& nearZ) noexcept;
        void setFarZ(const float& farZ) noexcept;
        void setPerspective(const float& fov) noexcept;
        void setOrthographic() noexcept;

        inline float getAspectRatio() const noexcept{
            return static_cast<float>(screenSize.x) / screenSize.y;
        }
    };
}