#pragma once

//= INCLUDES ========================
#include <array>
#include <memory>
#include "IComponent.h"
#include "../../Math/Vector4.h"
#include "../../Math/Vector3.h"
#include "../../Math/Matrix.h"
#include "../../RHI/RHI_Definition.h"
#include "../../Math/Frustum.h"
//===================================

using namespace Genome::Math;

namespace Genome
{
    class Camera;
    class Renderable;
    class Renderer;

    enum class LightType
    {
        Directional,
        Point,
        Spot
    };

    struct ShadowSlice
    {
        Vector3 min      = Vector3::Zero;
        Vector3 max      = Vector3::Zero;
        Vector3 center   = Vector3::Zero;
        Frustum frustum;
    };

    struct ShadowMap
    {
        std::shared_ptr<RHI_Texture> texture_color;
        std::shared_ptr<RHI_Texture> texture_depth;
        std::vector<ShadowSlice>     slices;
    };

    class GENOME_CLASS Light : public IComponent
    {
    public:
        Light(Context* context, Entity* entity, uint32_t id = 0);
        ~Light() = default;

        //= COMPONENT ================================
        void OnInitialize() override;
        void OnStart() override;
        void OnTick(float delta_time) override;
        void Serialize(FileStream* stream) override;
        void Deserialize(FileStream* stream) override;
        //============================================

        const auto GetLightType()                                const { return m_light_type; }
        void SetLightType(LightType type);

        void SetColor(const float temperature);
        void SetColor(const Vector4& rgb)                              { m_color_rgb = rgb; }
        const auto& GetColor()                                   const { return m_color_rgb; }

        void SetIntensity(float value)                                 { m_intensity = value; }
        auto GetIntensity()                                      const { return m_intensity; }

        bool GetShadowsEnabled()                                 const { return m_shadows_enabled; }
        void SetShadowsEnabled(bool cast_shadows);

        bool GetShadowsScreenSpaceEnabled()                      const { return m_shadows_screen_space_enabled; }
        void SetShadowsScreenSpaceEnabled(bool cast_contact_shadows)   { m_shadows_screen_space_enabled = cast_contact_shadows; }

        bool GetShadowsTransparentEnabled()                      const { return m_shadows_transparent_enabled; }
        void SetShadowsTransparentEnabled(bool cast_transparent_shadows);

        bool GetVolumetricEnabled()                              const { return m_volumetric_enabled; }
        void SetVolumetricEnabled(bool is_volumetric)                  { m_volumetric_enabled = is_volumetric; }

        void SetRange(float range);
        auto GetRange()                                          const { return m_range; }

        void SetAngle(float angle);
        auto GetAngle()                                          const { return m_angle_rad; }

        void SetTimeOfDay(float time_of_day);
        auto GetTimeOfDay()                                      const { return m_time_of_day; }

        void SetBias(float value)                                      { m_bias = value; }
        float GetBias()                                          const { return m_bias; }

        void SetNormalBias(float value)                                { m_normal_bias = value; }
        auto GetNormalBias()                                     const { return m_normal_bias; }

        const Matrix& GetViewMatrix(uint32_t index = 0)          const;
        const Matrix& GetProjectionMatrix(uint32_t index = 0)    const;

        RHI_Texture* GetDepthTexture()                           const { return m_shadow_map.texture_depth.get(); }
        RHI_Texture* GetColorTexture()                           const { return m_shadow_map.texture_color.get(); }
        uint32_t GetShadowArraySize()                            const;
        void CreateShadowMap();

        bool IsInViewFrustrum(Renderable* renderable, uint32_t index) const;

    private:
        void ComputeViewMatrix();
        bool ComputeProjectionMatrix(uint32_t index = 0);
        void ComputeCascadeSplits();

        // Shadows
        bool m_shadows_enabled              = true;
        bool m_shadows_screen_space_enabled = true;
        bool m_shadows_transparent_enabled  = true;
        uint32_t m_cascade_count            = 4;
        ShadowMap m_shadow_map;

        // Bias
        float m_bias        = 0.0f;
        float m_normal_bias = 3.0f;

        // Misc
        LightType m_light_type        = LightType::Directional;
        Vector4 m_color_rgb           = Vector4(1.0f, 0.76f, 0.57f, 1.0f);
        bool m_volumetric_enabled     = true;
        float m_range                 = 10.0f;
        float m_intensity             = 128000.0f;  // sun lux
        float m_angle_rad             = 0.5f;       // about 30 degrees
        float m_time_of_day           = 1.0f;
        bool m_initialized            = false;
        bool m_is_dirty               = true;
        std::array<Matrix, 6> m_matrix_view;
        std::array<Matrix, 6> m_matrix_projection;
        Quaternion m_previous_rot     = Quaternion::Identity;
        Vector3 m_previous_pos        = Vector3::Infinity;
        Matrix m_previous_camera_view = Matrix::Identity;
        Renderer* m_renderer;
    };
}
