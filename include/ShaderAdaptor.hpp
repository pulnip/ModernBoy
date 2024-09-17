#pragma once

#include <memory>
#include <span>
#include <string>
#include <vector>
#include <directxtk/SimpleMath.h>
#include "fwd.hpp"
#include "Attributes.hpp"
#include "ShaderConstant.hpp"
#include "Vertex.hpp"

namespace ModernBoy{
    class ShaderAdaptor{
        ComPtr<ID3D11VertexShader> vs;
        ComPtr<ID3D11PixelShader> ps;
        ComPtr<ID3D11InputLayout> il;
        ComPtr<ID3D11RasterizerState> rs;
        ComPtr<ID3D11RasterizerState> wireState;

        ComPtr<ID3D11Buffer> vertexBuffer;
        ComPtr<ID3D11Buffer> indexBuffer;
        ComPtr<ID3D11Buffer> vscBuffer;
        ComPtr<ID3D11Buffer> pscBuffer;

        UINT indexCount=0;

        VSConstants vsc;
        PSConstants psc;

        ComPtr<ID3D11InputLayout> nil;
        ComPtr<ID3D11VertexShader> nvs;
        ComPtr<ID3D11PixelShader> nps;

        ComPtr<ID3D11Buffer> nvb;
        ComPtr<ID3D11Buffer> nib;
        ComPtr<ID3D11Buffer> nvcBuffer;

        UINT nic=0;

        NormalConstants nvc;

        // Cube Mapping
        ComPtr<ID3D11InputLayout> cil;
        ComPtr<ID3D11VertexShader> cvs;
        ComPtr<ID3D11PixelShader> cps;

        ComPtr<ID3D11ShaderResourceView> crv;

        ComPtr<ID3D11Buffer> cvb;
        ComPtr<ID3D11Buffer> cib;
        ComPtr<ID3D11Buffer> cvcBuffer;

        UINT cic=0;

        CubeMappingConstants cmc;
        //

      public:
        std::shared_ptr<CameraComponent> mainCamera;
        std::vector<std::shared_ptr<LightComponent>> lights;
        size_t lightType=0;
        float normalScale=1.0f;
        float rimStrength=0.0f;
        float rimPower=1.0f;

      public:
        ShaderAdaptor(const ComPtr<Device>& device,
            const std::wstring& vsFileName, const std::wstring& psFileName);
        void loadMesh(const Mesh<Vertex>& mesh,
            const ComPtr<Device>& device
        );
        void loadNormal(const Mesh<Vertex>& mesh,
            const ComPtr<Device>& device
        );
        void draw(const Matrix& transform,
            const Material& material, const ComPtr<Context>& context
        );
        void setWireFrame(bool enable, const ComPtr<Context>& context);
        void render(const ComPtr<Context>& context);
        void renderNormal(const ComPtr<Context>& context);
    };
}
