#pragma once
#include "IComponent.h"
#include "..\..\RHI\RHI_Sampler.h"
#include "..\..\Math\Vector2.h"
#include <atomic>
#include <d3d11.h>
using namespace Genome::Math;

namespace Genome
{
    struct WaterParameters
    {
        int dmap_dim;
        float patch_length;
        float time_scale;
        float wave_amplitude;
        Vector2 wind_dir;
        float wind_speed;
        float wind_dependency;
        float choppy_scale;
    };

    enum class WaterType
    {
        River,
        Lake,
        Sea
    };

    class Model;

    namespace Math
    {
        class Vector3;
    }

    class GENOME_CLASS WaterComponent : public IComponent
    {
    public:
        WaterComponent(Context* context, Entity* entity, uint32_t id = 0);
        ~WaterComponent() = default;

        //= IComponent ===============================
        void OnInitialize() override;
        void Serialize(FileStream* stream) override;
        void Deserialize(FileStream* stream) override;
        //============================================

        const auto& GetHeightMap()            const { return m_height_map; }
        void SetHeightMap(const std::shared_ptr<RHI_Texture2D>& height_map);

        float GetMinY()                       const { return m_min_y; }
        void SetMinY(float min_z) { m_min_y = min_z; }

        float GetMaxY()                       const { return m_max_y; }
        void SetMaxY(float max_z) { m_max_y = max_z; }

        float GetProgress()                   const { return static_cast<float>(static_cast<double>(m_progress_jobs_done) / static_cast<double>(m_progress_job_count)); }
        const auto& GetProgressDescription()  const { return m_progress_desc; }

        void GenerateAsync();

    private:
        bool GeneratePositions(std::vector<Math::Vector3>& positions, const std::vector<std::byte>& height_map);
        bool GenerateVerticesIndices(const std::vector<Math::Vector3>& positions, std::vector<uint32_t>& indices, std::vector<RHI_Vertex_PosTexNorTan>& vertices);
        bool GenerateNormalTangents(const std::vector<uint32_t>& indices, std::vector<RHI_Vertex_PosTexNorTan>& vertices);
        void UpdateFromModel(const std::shared_ptr<Model>& model) const;
        void UpdateFromVertices(const std::vector<uint32_t>& indices, std::vector<RHI_Vertex_PosTexNorTan>& vertices);

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


