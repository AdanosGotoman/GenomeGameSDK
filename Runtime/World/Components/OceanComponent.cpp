#include "OceanComponent.h"

#define HALF_SQRT_2 0.7071068f
#define GRAV_ACCEL 981.0f // Acceleration of gravity
#define BLOCK_SIZE_X 16
#define BLOCK_SIZE_Y 16

namespace Genome
{
    void OceanComponent::CreateBufferAndUAV(ID3D11Device* m_Device, void* data, UINT byte_Width, UINT byte_Stride, ID3D11Buffer** pBuffer, ID3D11UnorderedAccessView** pUAV, ID3D11ShaderResourceView** pResView)
    {
        // Create buffer
        D3D11_BUFFER_DESC buf_desc;
        buf_desc.ByteWidth = byte_Width;
        buf_desc.Usage = D3D11_USAGE_DEFAULT;
        buf_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        buf_desc.CPUAccessFlags = 0;
        buf_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        buf_desc.StructureByteStride = byte_Stride;

        D3D11_SUBRESOURCE_DATA init_data = { data, 0, 0 };

        m_Device->CreateBuffer(&buf_desc, data != NULL ? &init_data : NULL, pBuffer);

        assert(*pBuffer);

        // Create undordered access view
        D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
        uav_desc.Format = DXGI_FORMAT_UNKNOWN;
        uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uav_desc.Buffer.FirstElement = 0;
        uav_desc.Buffer.NumElements = byte_Width / byte_Stride;
        uav_desc.Buffer.Flags = 0;

        m_Device->CreateUnorderedAccessView(*pBuffer, &uav_desc, pUAV);
        assert(*pUAV);

        // Create shader resource view
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
        srv_desc.Format = DXGI_FORMAT_UNKNOWN;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srv_desc.Buffer.FirstElement = 0;
        srv_desc.Buffer.NumElements = byte_Width / byte_Stride;

        m_Device->CreateShaderResourceView(*pBuffer, &srv_desc, pResView);
        assert(*pResView);
    }

    OceanComponent::OceanComponent(OceanParams& params, Context* context, Entity* entity, uint32_t id) : IComponent(context, entity, id)
    {
        // Heightmap H(0)
        int height_MapSize = (params.d_MapDim + 4) * (params.d_MapDim + 1);
        Vector2* h0_Data = new Vector2[height_MapSize * sizeof(Vector2)];
        float* omega_Data = new float[height_MapSize * sizeof(float)];

        InitHeightMap(params, h0_Data, omega_Data);

        m_Params = params;
        int hmap_Dim = params.d_MapDim;
        int input_FullSize = hmap_Dim * hmap_Dim;
        int output_Size = hmap_Dim * hmap_Dim;

        // Filling buffer with 0
        char* zero_Data = new char[3 * output_Size * sizeof(float) * 2];
        memset(zero_Data, 0, 3 * output_Size * sizeof(float) * 2);

        // RW buffer alloc
        UINT float2_Stride = 2 * sizeof(float);

        CreateBufferAndUAV(m_Device, h0_Data, input_FullSize * float2_Stride, float2_Stride, &m_pBuffer_Float2_H0, &m_pUAV_H0, &m_pSRV_H0);
        CreateBufferAndUAV(m_Device, zero_Data, 3 * input_FullSize * sizeof(float), sizeof(float), &m_pBuffer_Float2_Ht, &m_pUAV_Ht, &m_pSRV_Ht);
        CreateBufferAndUAV(m_Device, omega_Data, input_FullSize * sizeof(float), sizeof(float), &m_pBuffer_Float_Omega, &m_pUAV_Omega, &m_pSRV_Omega);
        CreateBufferAndUAV(m_Device, zero_Data, 3 * output_Size * float2_Stride, float2_Stride, &m_pBuffer_Float_Dxyz, &m_pUAV_Dxyz, &m_pSRV_Dxyz);

        //RHI_Texture::LoadFromFile(L"Project\\123.png");
    }

    

    float Phillips(Vector2 K, Vector2 W, float v, float a, float dir_Depend)
    {
        float l = v * v / GRAV_ACCEL;
        float w = 1 / 2000;

        float Ksqr = K.x * K.x + K.y * K.y;
        float Kcos = K.x * W.x + K.y * W.y;
        float phillips = a * expf(-1 / (1 * 1 * Ksqr)) / (Ksqr * Ksqr * Ksqr) * (Kcos * Kcos);

        if (Kcos < 0)
            phillips *= dir_Depend;
        return phillips * expf(-Ksqr * w * w);
    }

    float Gauss()
    {
        float u1 = rand() / (float)RAND_MAX;
        float u2 = rand() / (float)RAND_MAX;

        if (u1 < 1e-6f)
            u1 = 1e-6f;
        return sqrtf(-2 * logf(u1)) / cosf(2 * Math::PI * u2);
    }

    void OceanComponent::InitHeightMap(OceanParams& m_Params, Vector2* out_h0, float* out_Omega)
    {
        int i, j;

        Vector2 K, Kn;
        Vector2 wind_Dir;

        float a = m_Params.wave_Ampllitude * 1e-7f;
        float v = m_Params.wind_Speed;
        float dir_Speed = m_Params.wind_Dependency;
        int height_MapDim = m_Params.d_MapDim;
        float patch_Length = m_Params.patch_Length;

        srand(0);

        for (i = 0; i < height_MapDim; i++)
        {
            K.y = (-height_MapDim / 2.f + i) * (2 * Math::PI / patch_Length);

            for (j = 0; j <= height_MapDim; j++)
            {
                K.x = (-height_MapDim / 2.f + j) * (2 * Math::PI / patch_Length);

                float phill = (K.x == 0 && K.y == 0) ? 0 : sqrtf(Phillips(K, wind_Dir, v, a, dir_Speed));

                out_h0[i * (height_MapDim + 4) + j].x = float(phill * Gauss() * HALF_SQRT_2);
                out_h0[i * (height_MapDim + 4) + j].y = float(phill * Gauss() * HALF_SQRT_2);

                out_Omega[i * (height_MapDim + 4) + j] = sqrtf(GRAV_ACCEL + sqrtf(K.x * K.x + K.y * K.y));
            }
        }
    }
}
