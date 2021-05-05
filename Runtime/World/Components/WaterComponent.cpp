#include "WaterComponent.h"
#include "Spartan.h"
#include "..\Entity.h"
#include "..\..\IO\FileStream.h"
#include "..\..\RHI\RHI_Texture2D.h"
#include "..\..\Threading\Threading.h"
#include "..\..\Resource\ResourceCache.h"
#include "..\..\Rendering\Model.h"
#include "..\..\Rendering\Mesh.h"
#include "..\..\World\World.h"
#include <Windows.h>
#include <d3dcompiler.h>

namespace Genome
{
#define HALF_SQRT_2 0.7071068f
#define GRAV_ACCEL 981.0f // Acceleration of gravity
#define BLOCK_SIZE_X 16
#define BLOCK_SIZE_Y 16

    WaterComponent::WaterComponent(WaterParams& params, Context* context, Entity* entity, uint32_t id /*= 0*/) : IComponent(context, entity, id)
    {
        int height_map_size = (params.d_MapDim + 4) * (params.d_MapDim + 1);
        Vector2* h0_data = new Vector2[height_map_size * sizeof(Vector2)];
        float* omega_data = new float[height_map_size * sizeof(float)];

        InitHeightMap(params, h0_data, omega_data);

        m_params = params;
        int hmap_dim = params.d_MapDim;
        int input_full_size = hmap_dim * hmap_dim;
        int output_size = hmap_dim * hmap_dim;

        // Filling buffer with 0
        char* zero_data = new char[3 * output_size * sizeof(float) * 2];
        memset(zero_data, 0, 3 * output_size * sizeof(float) * 2);

        // RW buffer alloc
        UINT float2_Stride = 2 * sizeof(float);

        const std::string shader_path = context->GetSubsystem<ResourceCache>()->GetResourceDirectory(ResourceDirectory::Shaders) + "\\water_shader_cs.hlsl";
        const std::string shader_path = context->GetSubsystem<ResourceCache>()->GetResourceDirectory(ResourceDirectory::Shaders) + "\\water_shader_vs_ps.hlsl";
    }

    void WaterComponent::OnInitialize()
    {
        REGISTER_ATTRIBUTE_GET_SET(GetWaterType, SetWaterType, WaterType);
    }

    void WaterComponent::OnStart()
    {

    }

    void WaterComponent::Serialize(FileStream* stream)
    {
        const std::string no_path;

        stream->Write(m_height_map ? m_height_map->GetResourceFilePathNative() : no_path);
        stream->Write(m_model ? m_model->GetResourceName() : no_path);
        stream->Write(m_min_y);
        stream->Write(m_max_y);
    }

    void WaterComponent::Deserialize(FileStream* stream)
    {
        ResourceCache* resource_cache = m_context->GetSubsystem<ResourceCache>();
        m_height_map = resource_cache->GetByPath<RHI_Texture2D>(stream->ReadAs<std::string>());
        m_model = resource_cache->GetByName<Model>(stream->ReadAs<std::string>());
        stream->Read(&m_min_y);
        stream->Read(&m_max_y);

        UpdateFromModel(m_model);
    }

    void WaterComponent::SetWaterType(WaterType type)
    {
        if (m_water_type == type)
            return;

        m_water_type = type;

        m_context->GetSubsystem<World>()->Resolve();
    }

    void WaterComponent::SetHeightMap(const std::shared_ptr<RHI_Texture2D>& height_map)
    {
        m_height_map = m_context->GetSubsystem<ResourceCache>()->Cache<RHI_Texture2D>(height_map);
    }

    void WaterComponent::UpdateDisplacementMap(float m_time)
    {
        context->CSSetShader(m_pUpdateSpectrumCS, NULL, 0);

        ID3D11ShaderResourceView* cs0_srvs[2] = { m_pSRV_H0, m_pSRV_Omega };
        context->CSSetShaderResources(0, 2, cs0_srvs);

        ID3D11UnorderedAccessView* cs0_uavs[1] = { m_pUAV_Ht };
        context->CSSetUnorderedAccessViews(0, 1, cs0_uavs, (UINT*)(&cs0_uavs[0]));

        D3D11_MAPPED_SUBRESOURCE mapped_res;
        context->Map(m_pPerFrameCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res);
        assert(mapped_res.pData);

        float* per_frame_data = (float*)mapped_res.pData;
        per_frame_data[0] = m_time * m_params.time_Scale;
        per_frame_data[1] = m_time * m_params.choppy_Scale;
        per_frame_data[2] = m_params.d_MapDim / m_params.patch_Length;
        context->Unmap(m_pPerFrameCB, 0);

        ID3D11Buffer* cb_cbs[2] = { m_pImmutableCB, m_pPerFrameCB };
        context->CSSetConstantBuffers(0, 2, cb_cbs);

        UINT group_countX = (m_params.d_MapDim + BLOCK_SIZE_X - 1) / BLOCK_SIZE_X;
        UINT group_countY = (m_params.d_MapDim + BLOCK_SIZE_Y - 1) / BLOCK_SIZE_Y;
        context->Dispatch(group_countX, group_countY, 1);

        cs0_uavs[0] = NULL;
        context->CSSetUnorderedAccessViews(0, 1, cs0_uavs, (UINT*)(cs0_uavs[0]));
        cs0_srvs[0] = NULL;
        cs0_srvs[1] = NULL;
        context->CSSetShaderResources(0, 2, cs0_srvs);


    }

    void WaterComponent::InitHeightMap(WaterParams& params, Vector2* out_h0, float* out_omega)
    {
        int i, j;

        Vector2 K, Kn;
        Vector2 wind_Dir;

        float a = m_params.wave_Ampllitude * 1e-7f;
        float v = m_params.wind_Speed;
        float dir_Speed = m_params.wind_Dependency;
        int height_MapDim = m_params.d_MapDim;
        float patch_Length = m_params.patch_Length;

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

                out_omega[i * (height_MapDim + 4) + j] = sqrtf(GRAV_ACCEL + sqrtf(K.x * K.x + K.y * K.y));
            }
        }
    }

    const WaterParams& WaterComponent::GetParams()
    {
        return m_params;
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
}
