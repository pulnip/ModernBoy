#pragma once

#include "Attributes.hpp"

namespace ModernBoy{
    class Camera{
        Transform transform;

        DirectX::SimpleMath::Matrix getView() const noexcept;
    };
}