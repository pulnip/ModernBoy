#include "Attributes.hpp"

using namespace std;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

Matrix Transform::transform() const{
    lock_guard lg(mtx);

    const auto translation=Matrix::CreateTranslation(position);
    const auto rotation=Matrix::CreateFromQuaternion(quaternion);
    const auto scaling=Matrix::CreateScale(scale);

    return scaling*rotation*translation;
}
