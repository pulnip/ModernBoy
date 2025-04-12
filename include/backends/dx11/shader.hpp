#ifndef __INC_DX11_SHADER_HPP
#define __INC_DX11_SHADER_HPP

#include <span>
#include <stdexcept>
#include <string>
#include "type.hpp"
#include "resource_data.hpp"

#define MAX_LIGHTS 3

namespace ModernBoy::DX11
{
    struct DefaultVSConstants{
        DirectX::XMFLOAT4X4 model{};
        DirectX::XMFLOAT4X4 invTranspose{};
        DirectX::XMFLOAT4X4 view{};
        DirectX::XMFLOAT4X4 projection{};
    }; ALIGN16(DefaultVSConstants);
    struct DefaultPSConstants{
        // float xSplit=0.0f;
        // float dummy[3]={0.0f, 0.0f, 0.0f};
        DirectX::XMFLOAT3 eyePos;
        bool useTexture;
        Material material;
        Light lights[MAX_LIGHTS];
        // Rim Lighting
        DirectX::XMFLOAT3 rimColor{1.0f, 1.0f, 1.0f};
        float rimStrength;
        float rimPower;
        float dummy[3];
    }; ALIGN16(DefaultPSConstants);

    struct NormalConstants{
        float scale;
        float dummy[3];
    }; ALIGN16(NormalConstants);

    // Not intended for Direct use.
    namespace detail{
        bool InitRasterizer(DevicePtr& in_device, D3D11_FILL_MODE in_fillMode,
            RasterizerPtr& out_rs);
        template<typename T>
        bool InitConstantBuffer(DevicePtr& in_device, const T& in_constants,
            BufferPtr& out_buffer);
        bool InitShader(const std::wstring& in_vsFileName,
            const std::wstring& in_psFileName, DevicePtr& in_device,
            std::span<const D3D11_INPUT_ELEMENT_DESC> in_iedesc,
            InputLayoutPtr& out_il, VertexShaderPtr& out_vs,
            PixelShaderPtr& out_ps);

        template<typename VSConstants, typename PSConstants>
        struct Shader{
            RasterizerPtr rs;

            InputLayoutPtr il;
            // Vertex Shader
            VSConstants vsc;
            BufferPtr vscBuffer;
            VertexShaderPtr vs;
            // Pixel Shader
            PSConstants psc;
            BufferPtr pscBuffer;
            PixelShaderPtr ps;

        public:
            Shader()=default;
            ~Shader()=default;
            Shader(const Shader&)=delete;
            Shader(Shader&& other){ moveFrom(std::move(other)); }
            Shader& operator=(const Shader&)=delete;
            Shader& operator=(Shader&& other){
                moveFrom(std::move(other));
                return *this;
            }

            Shader(DevicePtr& in_device,
                const std::wstring& in_vsFileName = L"shaders/VS.hlsl",
                const std::wstring& in_psFileName = L"shaders/PS.hlsl")
            {
                // Init Default Rasterizer
                if(!InitRasterizer(in_device, D3D11_FILL_SOLID, rs))
                    throw std::runtime_error("Failed to create "
                        "rasterizer state.");
                // Init Vertex Shader Constants
                if(!InitConstantBuffer(in_device, vsc, vscBuffer))
                    throw std::runtime_error("Failed to create "
                        "vertex shader constant buffer.");
                // Init Pixel Shader Constants
                if(!InitConstantBuffer(in_device, psc, pscBuffer))
                    throw std::runtime_error("Failed to create "
                        "pixel shader constant buffer.");

                // Init shaders
                if(!InitShader(in_vsFileName, in_psFileName,
                    in_device, smolVertexDesc, il, vs, ps))
                    throw std::runtime_error("Failed to create shaders.");
            }

            void bind(RenderContext& in_context) const;

        private:
            // Move semantics
            void moveFrom(Shader&& other){
                rs.Swap(other.rs);

                il.Swap(other.il);
                vsc = other.vsc;
                vscBuffer.Swap(other.vscBuffer);
                vs.Swap(other.vs);
                psc = other.psc;
                pscBuffer.Swap(other.pscBuffer);
                ps.Swap(other.ps);
            }
        };
    }

    using DefaultShader=detail::Shader<DefaultVSConstants, DefaultPSConstants>;
    // static_assert(ResourceData<DefaultShader>);
    using NormalShader=detail::Shader<NormalConstants, DefaultPSConstants>;
    // static_assert(ResourceData<NormalShader>);
}

#endif // __INC_DX11_SHADER_HPP