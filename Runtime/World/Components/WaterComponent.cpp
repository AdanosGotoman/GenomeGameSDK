#include "Spartan.h"
#include "WaterComponent.h"
#include "Renderable.h"
#include "..\Entity.h"
#include "..\..\RHI\RHI_Texture2D.h"
#include "..\..\RHI\RHI_Vertex.h"
#include "..\..\Rendering\Model.h"
#include "..\..\IO\FileStream.h"
#include "..\..\Resource\ResourceCache.h"
#include "..\..\Rendering\Mesh.h"
#include "..\..\Threading\Threading.h"

using namespace Genome::Math;

namespace Genome
{
    WaterComponent::WaterComponent(Context* context, Entity* entity, uint32_t id /*=0*/) : IComponent(context, entity, id)
    {

    }

    void WaterComponent::OnInitialize()
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

    void WaterComponent::SetHeightMap(const std::shared_ptr<RHI_Texture2D>& height_map)
    {
        // In order for the component to guarantee serialization/deserialization, we cache the height_map
        m_height_map = m_context->GetSubsystem<ResourceCache>()->Cache<RHI_Texture2D>(height_map);
    }

    void WaterComponent::GenerateAsync()
    {
        if (m_is_generating)
        {
            LOG_WARNING("Terrain is already being generated, please wait...");
            return;
        }

        if (!m_height_map)
        {
            LOG_WARNING("You need to assign a height map before trying to generate a terrain.");

            m_context->GetSubsystem<ResourceCache>()->Remove(m_model);
            m_model.reset();
            if (Renderable* renderable = m_entity->AddComponent<Renderable>())
            {
                renderable->GeometryClear();
            }

            return;
        }

        m_context->GetSubsystem<Threading>()->AddTask([this]()
            {
                m_is_generating = true;

                // Get height map data
                const std::vector<std::byte> height_map_data = m_height_map->GetOrLoadMip(0);
                if (height_map_data.empty())
                {
                    LOG_ERROR("Height map has no data");
                }

                // Deduce some stuff
                m_height = m_height_map->GetHeight();
                m_width = m_height_map->GetWidth();
                m_vertex_count = m_height * m_width;
                m_face_count = (m_height - 1) * (m_width - 1) * 2;
                m_progress_jobs_done = 0;
                m_progress_job_count = m_vertex_count * 2 + m_face_count + m_vertex_count * m_face_count;

                // Pre-allocate memory for the calculations that follow
                std::vector<Vector3> positions = std::vector<Vector3>(m_height * m_width);
                std::vector<RHI_Vertex_PosTexNorTan> vertices = std::vector<RHI_Vertex_PosTexNorTan>(m_vertex_count);
                std::vector<uint32_t> indices = std::vector<uint32_t>(m_face_count * 3);

                // Read height map and construct positions
                m_progress_desc = "Generating positions...";
                if (GeneratePositions(positions, height_map_data))
                {
                    // Compute the vertices (without the normals) and the indices
                    m_progress_desc = "Generating terrain vertices and indices...";
                    if (GenerateVerticesIndices(positions, indices, vertices))
                    {
                        m_progress_desc = "Generating normals and tangents...";
                        positions.clear();
                        positions.shrink_to_fit();

                        // Compute the normals by doing normal averaging (very expensive)
                        if (GenerateNormalTangents(indices, vertices))
                        {
                            // Create a model and set it to the renderable component
                            UpdateFromVertices(indices, vertices);
                        }
                    }
                }

                // Clear progress stats
                m_progress_jobs_done = 0;
                m_progress_job_count = 1;
                m_progress_desc.clear();

                m_is_generating = false;
            });
    }

}
