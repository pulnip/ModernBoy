#include <algorithm>
#include <cassert>
#include <random>

#include "Math.hpp"

namespace My{
    namespace Math{
        auto Angle::Degree::toRadian() noexcept{
            return Radian{(Number::PI/180.0) * angle};
        }
        auto Angle::Radian::toDegree() noexcept{
            return Degree{(180.0/Number::PI) * angle};
        }


        Number::Int wrap(const Number::Int target,
            const Number::Int floor, const Number::Int ceil
        ) noexcept{
            assert(floor < ceil);
            const auto height=ceil-floor;
            const auto distance=target-floor;
            const auto remain=distance%height;
            return remain>=0 ? floor+remain : ceil+remain;
        }
        Number::Real wrap(const Number::Real target,
            const Number::Real floor, const Number::Real ceil
        ) noexcept{
            assert(floor < ceil);
            const auto height=ceil-floor;
            const auto distance=target-floor;
            const auto remain=std::fmod(distance, height);
            return remain>=0 ? floor+remain : ceil+remain;
        }

        Number::Int random(
            const Number::Int start, const Number::Int end) noexcept
        {
            // 난수 생성 엔진
            static std::mt19937 gen(
                // 시드값을 얻기 위해
                (std::random_device())());
            // [start, end) 균등 분포
            std::uniform_int_distribution dis(start, end);

            return dis(gen);
        }
        Number::Real random(
            const Number::Real start, const Number::Real end) noexcept
        {
            // 난수 생성 엔진
            static std::mt19937 gen(
                // 시드값을 얻기 위해
                (std::random_device())());
            // [start, end) 균등 분포
            std::uniform_real_distribution dis(start, end);

            return dis(gen);
        }

        Vector2<Number::Real> forward(Angle::Radian radian) noexcept{
            return Vector2<Number::Real>{
                static_cast<float>(std::cos(radian.get())),
                static_cast<float>(std::sin(radian.get()))};
        }
        Angle::Radian rotation(const Vector2<Number::Real>& v) noexcept{
            return Angle::Radian{std::atan2(v.y, v.x)};
        }
    }
}