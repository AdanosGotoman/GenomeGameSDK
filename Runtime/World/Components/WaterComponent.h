#pragma once
#include "Spartan.h"
#include "IComponent.h"

using namespace Genome::Math;

namespace Genome
{
    class Model;

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

    enum class WaterType
    {
        River,
        Lake,
        Ocean
    };

    class GENOME_CLASS OceanComponent : public IComponent
    {
    public:
        OceanComponent(Context* context, Entity* entity, uint32_t id = 0);
        ~OceanComponent() = default;

        OnInitialize() override;
        OnStart() override;
        OnTick(float time) override;
        Serialize(FileStream* stream) override;
        Deserialize(FileStream* stream) override;

        const auto GetWaterType() const { return m_water_type; }
        void SetWaterType(WaterType type);

        const auto GetHeightMap() const { return m_height_map; }
        void SetHeightMap(const std::shared_ptr<RHI_Texture2D>& height_map);

        float GetMinY() const { return m_min_y; }
        float GetMaxY() const { return m_max_y; }

        void SetMinY(float min_z) { m_min_y = min_z; }
        void SetMaxY(float max_z) { m_max_y = max_z; }

        void GenerateAsync();

    private:
        bool GeneratePositions(std::vector<Math::Vector3>& positions, const std::vector<std::byte>& height_map);
        bool GenerateVerticesIndices(const std::vector<Math::Vector3>& positions, std::vector<uint32_t>& indices, std::vector<RHI_Vertex_PosTexNorTan>& vertices);
        bool GenerateNormalTangents(const std::vector<uint32_t>& indices, std::vector<RHI_Vertex_PosTexNorTan>& vertices);
        void UpdateFromModel(const std::shared_ptr<Model>& model) const;
        void UpdateFromVertices(const std::vector<uint32_t>& indices, std::vector<RHI_Vertex_PosTexNorTan>& vertices);

    private:
        WaterType m_water_type;
        uint32_t m_width = 0;
        uint32_t m_height = 0;
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


