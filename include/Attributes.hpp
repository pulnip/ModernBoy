#pragma once

#include <mutex>
#include <print>
#include <tuple>
#include <vector>
#include <directxtk/SimpleMath.h>
#include <directxmath/DirectXColors.h>
#include "Math.hpp"

namespace ModernBoy{
    struct Transform{
        Vector3 position=Vector3::Zero;
        Quaternion quaternion=Quaternion::Identity;
        Vector3 scale=Vector3::One;

        Matrix transform() const;

      private:
        mutable std::mutex mtx;
    };

    struct Material{
        Vector3 ambient{0.1f, 0.1f, 0.1f};
        float shininess=1.0f;
        Vector3 diffuse{0.5f, 0.5f, 0.5f};
        float reflection=0.0f;
        Vector3 specular{0.5f, 0.5f, 0.5f};
        float transparency=0.0f;
    };
    // check 16-byte aligned
    static_assert((sizeof(Material) & 0xf) == 0);

    struct Light{
        Vector3 strength=Vector3::One;
        float fallOffStart=0.0f;
        Vector3 dir={0.0f, -0.70711f, 0.70711f};
        float fallOffEnd=10.0f;
        Vector3 pos={0.0f, 2.0f, -2.0f};
        float spotPower=1.0f;
    };
    // check 16-byte aligned
    static_assert((sizeof(Light) & 0xf) == 0);

    struct Polygon{
        size_t index[3];
    };

    template<typename V> struct Mesh{
        using VertexBuffer=std::vector<V>;
        using IndexBuffer=std::vector<uint16_t>;

        VertexBuffer verticies;
        std::vector<Polygon> polygons;

        std::tuple<VertexBuffer, IndexBuffer> extract() const{
            IndexBuffer indices;

            indices.reserve(3*polygons.size());
            for(const auto& polygon: polygons){
                indices.emplace_back(polygon.index[0]);
                indices.emplace_back(polygon.index[1]);
                indices.emplace_back(polygon.index[2]);
            }

            return {verticies, indices};
        }

        std::tuple<VertexBuffer, IndexBuffer> extractNormal() const{
            VertexBuffer nv;
            IndexBuffer ni;
            nv.reserve(2*verticies.size());
            ni.reserve(2*verticies.size());

            uint16_t i=0;
            for(const auto& vertex: verticies){
                V normalStart{
                    .position=vertex.position,
                    .normal=vertex.normal,
                    .uv={0.0f, 0.0f}
                };
                V normalEnd{
                    .position=vertex.position,
                    .normal=vertex.normal,
                    .uv={1.0f, 0.0f}
                };
                nv.emplace_back(normalStart);
                nv.emplace_back(normalEnd);
                ni.emplace_back(i);
                ni.emplace_back(i+1);

                i+=2;
            }

            return {nv, ni};
        };
    };
}
