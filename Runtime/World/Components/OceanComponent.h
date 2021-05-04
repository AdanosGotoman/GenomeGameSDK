#pragma once
#include "IComponent.h"
#include "Renderable.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../Math/Vector2.h"
#include "../../RHI/RHI_VertexBuffer.h"
#include "../../RHI/RHI_Texture2D.h"

using namespace DirectX;
using namespace Genome::Math;

namespace Genome
{
    struct OceanParams
    {
        int d_MapDim;
        float patch_Length;
        float time_Scale;
        float wave_Ampllitude;
        float wind_Dependency;
        float wind_Speed;
        float choppy_Scale;
        Vector2 wind_Dir;
    };

    class GENOME_CLASS OceanComponent : public IComponent
    {
    public:
        void CreateBufferAndUAV(ID3D11Device* m_Device, void* data, UINT byte_Width, UINT byte_Stride, ID3D11Buffer** pBuffer, ID3D11UnorderedAccessView** pUAV, ID3D11ShaderResourceView** pResView);
        OceanComponent(OceanParams& params, Context* context, Entity* entity, uint32_t id = 0);
        ~OceanComponent() = default;

        void UpdateDisplacementMap();
        void UseDefaultMaterial();
        ID3D11ShaderResourceView* GetDisplacementMap();
        ID3D11ShaderResourceView* GetGradientMap();

        const OceanParams& GetParams();

    protected:
        OceanParams m_Params;

        ID3D11Device* m_Device;
        RHI_VertexBuffer* m_Buffer;
        ID3D11DeviceContext* m_Context;
        RHI_Texture2D* m_DisplacementMap;
        ID3D11ShaderResourceView* m_DisplacementSRV;
        ID3D11RenderTargetView* m_DisplacementRTV;

        RHI_Texture2D* m_GradientMap;
        RHI_Sampler* m_Sampler;

        void InitHeightMap(OceanParams& m_Params, Vector2* out_h0, float* out_Omega);

        ID3D11Buffer* m_pBuffer_Float2_H0;
        ID3D11UnorderedAccessView* m_pUAV_H0;
        ID3D11ShaderResourceView* m_pSRV_H0;

        // Angular frequency
        ID3D11Buffer* m_pBuffer_Float_Omega;
        ID3D11UnorderedAccessView* m_pUAV_Omega;
        ID3D11ShaderResourceView* m_pSRV_Omega;

        // Height field H(t), choppy field Dx(t) and Dy(t) in frequency domain, updated each frame.
        ID3D11Buffer* m_pBuffer_Float2_Ht;
        ID3D11UnorderedAccessView* m_pUAV_Ht;
        ID3D11ShaderResourceView* m_pSRV_Ht;

        // Height & choppy buffer in the space domain, corresponding to H(t), Dx(t) and Dy(t)
        ID3D11Buffer* m_pBuffer_Float_Dxyz;
        ID3D11UnorderedAccessView* m_pUAV_Dxyz;
        ID3D11ShaderResourceView* m_pSRV_Dxyz;

        ID3D11Buffer* m_pQuadVB;

        // Shaders, layouts and constants
        ID3D11ComputeShader* m_pUpdateSpectrumCS;

        ID3D11VertexShader* m_pQuadVS;
        ID3D11PixelShader* m_pUpdateDisplacementPS;
        ID3D11PixelShader* m_pGenGradientFoldingPS;

        ID3D11InputLayout* m_pQuadLayout;

        ID3D11Buffer* m_pImmutableCB;
        ID3D11Buffer* m_pPerFrameCB;
    };
}


