#pragma once
#include "IComponent.h"
#include "..\..\RHI\RHI_Implementation.h"
#include <vector>

using namespace Genome::Math;

namespace Genome
{
    struct WaterParams
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

    enum class WaterType
    {
        River,
        Lake,
        Ocean
    };

    class GENOME_CLASS WaterComponent : public IComponent
    {
    public:
        WaterComponent(WaterParams& params, Context* context, Entity* entity, uint32_t id = 0);
        ~WaterComponent() = default;

        void OnInitialize() override;
        void OnStart() override;
        void OnTick(float time) override;
        void Serialize(FileStream* stream) override;
        void Deserialize(FileStream* stream) override;

        const auto GetWaterType() const { return m_water_type; }
        void SetWaterType(WaterType type);

        const auto GetHeightMap() const { return m_height_map; }
        void SetHeightMap(const std::shared_ptr<RHI_Texture2D>& height_map);

        float GetMinY() const { return m_min_y; }
        float GetMaxY() const { return m_max_y; }

        void SetMinY(float min_z) { m_min_y = min_z; }
        void SetMaxY(float max_z) { m_max_y = max_z; }

        void GenerateAsync();
        void UpdateDisplacementMap(float m_time);
        void InitHeightMap(WaterParams& params, Vector2* out_h0, float* out_omega);
        const WaterParams& GetParams();

    private:
        bool GeneratePositions(std::vector<Math::Vector3>& positions, const std::vector<std::byte>& height_map);
        bool GenerateVerticesIndices(const std::vector<Math::Vector3>& positions, std::vector<uint32_t>& indices, std::vector<RHI_Vertex_PosTexNorTan>& vertices);
        bool GenerateNormalTangents(const std::vector<uint32_t>& indices, std::vector<RHI_Vertex_PosTexNorTan>& vertices);
        void UpdateFromModel(const std::shared_ptr<Model>& model) const;
        void UpdateFromVertices(const std::vector<uint32_t>& indices, std::vector<RHI_Vertex_PosTexNorTan>& vertices);

    public:
        WaterParams m_params;
        WaterType m_water_type;
        ID3D11DeviceContext1* context;
        uint32_t m_width = 0;
        uint32_t m_height = 0;

        ID3D11ComputeShader* m_pUpdateSpectrumCS;
        ID3D11ShaderResourceView* m_pSRV_H0;
        ID3D11ShaderResourceView* m_pSRV_Ht;
        ID3D11ShaderResourceView* m_pSRV_Omega;
        ID3D11ShaderResourceView* m_pSRV_Dxyz;
        ID3D11UnorderedAccessView* m_pUAV_Ht;
        ID3D11UnorderedAccessView* m_pUAV_Dxyz;
        ID3D11Buffer* m_pPerFrameCB;
        ID3D11Buffer* m_pImmutableCB;

        static std::unique_ptr<RHI_Shader> shader;

        float m_min_y = 0.0f;
        float m_max_y = 30.0f;
        float m_vertex_density = 1.0f;
        std::atomic<bool> m_is_generating = false;
        uint64_t m_vertex_count = 0;
        uint64_t m_face_count = 0;
        std::atomic<uint64_t> m_progress_jobs_done = 0;
        uint64_t m_progress_job_count = 1; // avoid devision by zero in GetProgress()
        std::string m_progress_desc;
        std::shared_ptr<RHI_Texture2D> m_height_map;
        std::shared_ptr<Model> m_model;
    };
}


