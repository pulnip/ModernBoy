#pragma once

#include <cassert>
#include <cmath>
#include <limits>

#include "Concept.hpp"

namespace My{
    namespace Math{
        namespace Number{
            using Int = int;
            using Real = double;

#if __INTELLISENSE__
            constexpr Real PI = 3.14159265;
#else
            constexpr Real PI = std::asin(1) * 2;
#endif
            constexpr Real infinity=std::numeric_limits<double>::infinity();
            constexpr Real epsilon=0.0001;
            
            bool NearZero(Numeric auto number) noexcept{
                return std::abs(number) < epsilon;
            }
        }

        namespace Angle{
            struct Radian;

            struct Degree{
                Number::Real angle;
                Degree(Number::Real angle) noexcept:angle(angle){}
                Radian toRadian() const noexcept;
                constexpr auto& get() noexcept{ return angle; }
                constexpr const auto& get() const noexcept{ return angle; }
            };
            struct Radian{
                Number::Real angle;
                Radian(Number::Real angle) noexcept:angle(angle){}
                Degree toDegree() const noexcept;
                constexpr auto& get() noexcept{ return angle; }
                constexpr const auto& get() const noexcept{ return angle; }
            };
        }

        template <Numeric Number>
        Number reflect(const Number target, const Number base) noexcept{
            return 2 * base - target;
        }

        template <Numeric Number>
        bool inClosedRange(const Number target,
            const Number start, const Number end
        ) noexcept{
            assert(start <= end);
            return start <= target and target <= end;
        }
        template <Numeric Number>
        bool inOpenedRange(const Number target,
            const Number start, const Number end
        ) noexcept{
            assert(start < end);
            return start < target and target < end;
        }
        template <Numeric Number>
        bool inCloseOpenedRange(const Number target,
            const Number start, const Number end
        ) noexcept{
            assert(start < end);
            return start <= target and target < end;
        }

        Number::Int wrapSmall(const Number::Int target,
            const Number::Int floor, const Number::Int ceil
        ) noexcept;
        Number::Int wrap(const Number::Int target,
            const Number::Int floor, const Number::Int ceil
        ) noexcept;
        Number::Real wrap(const Number::Real target,
            const Number::Real floor, const Number::Real ceil
        ) noexcept;

        Number::Real random(const Number::Real start, const Number::Real end) noexcept;

        template<Numeric Number> struct Vector2{
            Number x=0, y=0;
    
          public:
            Vector2() noexcept=default;
            Vector2(Number x, Number y):x(x), y(y){}
            template<Numeric Other>
                requires (not std::same_as<Number, Other>)
            Vector2(const Vector2<Other>& other)
            :x(static_cast<Number>(other.x))
            ,y(static_cast<Number>(other.y)){}
            template<Numeric Other>
                requires (not std::same_as<Number, Other>)
            Vector2(Other x, Other y)
            :x(static_cast<Number>(x))
            ,y(static_cast<Number>(y)){}

            Vector2& operator+=(const Vector2 &other) noexcept{
                x += other.x; y += other.y;
                return *this;
            }
            Vector2& operator-=(const Vector2 &other) noexcept{
                x -= other.x; y -= other.y;
                return *this;
            }
        };

        template<Numeric Number> using Linear = Vector2<Number>;

        template<Numeric Number>
        Vector2<Number> abs(const Vector2<Number>& v) noexcept{
            return Vector2<Number>{std::abs(v.x), std::abs(v.y)};
        }

        template<Numeric Number>
        Number::Real size(const Vector2<Number>& v) noexcept{
            return std::sqrt(v.x*v.x + v.y*v.y);
        }

        template<Numeric N>
        Vector2<Number::Real> normalize(const Vector2<N>& v) noexcept{
            return v / size(v);
        }

        Vector2<Number::Real> forward(Angle::Radian radian) noexcept;
        Angle::Radian rotation(const Vector2<Number::Real>& v) noexcept;

        template<Numeric Number> Vector2<Number> operator+(
            const Vector2<Number>& lhs, const Vector2<Number>& rhs
        ) noexcept{
            return Vector2<Number>{lhs.x+rhs.x, lhs.y+rhs.y};
        }
        template<Numeric Number> Vector2<Number> operator-(
            const Vector2<Number>& lhs, const Vector2<Number>& rhs
        ) noexcept{
            return Vector2<Number>{lhs.x-rhs.x, lhs.y-rhs.y};
        }
        template<Numeric Number> Vector2<Number> operator*(
            const Number scalar, const Vector2<Number>& v
        ) noexcept{
            return Vector2<Number>{scalar*v.x, scalar*v.y};
        }
        template<Numeric Number> Vector2<Number> operator*(
            const Vector2<Number>& v, const Number scalar
        ) noexcept{
            return scalar*v;
        }
        // dot product
        template<Numeric Number> Number dot(
            const Vector2<Number>& lhs, const Vector2<Number> &rhs
        ) noexcept{
            return lhs.x*rhs.x + lhs.y*rhs.y;
        }
        template<Numeric N> Vector2<N> operator/(
            const Vector2<N>& v, const N scalar
        ) noexcept{
            assert(not Number::NearZero(scalar));
            return Vector2<N>{v.x/scalar, v.y/scalar};
        }
        template<Numeric Number> bool operator<=(
            const Vector2<Number>& lhs, const Vector2<Number>& rhs
        ) noexcept{
            return lhs.x<=rhs.x and lhs.y<=rhs.y;
        }
    } // namespace Math
}