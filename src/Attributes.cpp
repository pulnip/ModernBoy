#include "Attributes.hpp"

using namespace DirectX::SimpleMath;
using namespace ModernBoy;

Matrix Transform::transform() const{
    std::lock_guard lg(mtx);

    const auto translation=Matrix::CreateTranslation(position);
    const auto rotation=Matrix::CreateFromQuaternion(quaternion);
    const auto scaling=Matrix::CreateScale(scale);

    return scaling*rotation*translation;
}
