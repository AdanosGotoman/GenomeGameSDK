/*
Copyright(c) 2016-2021 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES ========================
#include "IComponent.h"
#include <memory>
#include "../../RHI/RHI_Definition.h"
#include "../../RHI/RHI_Viewport.h"
#include "../../Math/Rectangle.h"
#include "../../Math/Matrix.h"
#include "../../Math/Ray.h"
#include "../../Math/Frustum.h"
#include "../../Math/Vector2.h"
#include <DirectXMath.h>
using namespace DirectX;
//===================================

using namespace Genome::Math;

namespace Genome
{
    class Entity;
    class Model;
    class Renderable;
    class Input;
    class Renderer;

    enum ProjectionType
    {
        Projection_Perspective,
        Projection_Orthographic,
    };

    class GENOME_CLASS Camera : public IComponent
    {
    public:
        Camera(Context* context, Entity* entity, uint32_t id = 0);
        ~Camera() = default;

        //= ICOMPONENT ===============================
        void OnInitialize() override;
        void OnTick(float delta_time) override;
        void Serialize(FileStream* stream) override;
        void Deserialize(FileStream* stream) override;
        //============================================

        //= MATRICES ======================================================================
        const Matrix& GetViewMatrix()             const { return m_view; }
        const Matrix& GetProjectionMatrix()       const { return m_projection; }
        const Matrix& GetViewProjectionMatrix()   const { return m_view_projection; }
        //=================================================================================

        //= RAYCASTING =================================================================
        // Returns the ray the camera uses to do picking
        const Ray& GetPickingRay() const { return m_ray; }

        // Picks the nearest entity under the mouse cursor
        bool Pick(const Vector2& mouse_position, std::shared_ptr<Entity>& entity);

        // Converts a world point to a screen point
        Vector2 Project(const Vector3& position_world) const;

        // Converts a world bounding box to a screen rectangle
        Math::Rectangle Project(const BoundingBox& bounding_box) const;

        // Converts a screen point to a world point
        Vector3 Unproject(const Vector2& position_screen) const;
        //==============================================================================

        float GetAperture()                       const { return m_aperture; }
        void SetAperture(const float aperture)          { m_aperture = aperture; }

        float GetShutterSpeed()                   const { return m_shutter_speed; }
        void SetShutterSpeed(const float shutter_speed) { m_shutter_speed = shutter_speed; }

        float GetIso()                            const { return m_iso; }
        void SetIso(const float iso)                    { m_iso = iso; }

        // Reference: https://google.github.io/filament/Filament.md.html#lighting/units/lightunitsvalidation
        float GetEv100()    const { return std::log2((m_aperture * m_aperture) / m_shutter_speed * 100.0f / m_iso); }
        // Frostbite: https://seblagarde.files.wordpress.com/2015/07/course_notes_moving_frostbite_to_pbr_v32.pdf
        float GetExposure() const { return 1.0f / (std::pow(2.0f, GetEv100()) * 1.2f); }

        //= PLANES/PROJECTION =================================================
        void SetNearPlane(float near_plane);
        void SetFarPlane(float far_plane);
        void SetProjection(ProjectionType projection);
        float GetNearPlane()                      const { return m_near_plane; }
        float GetFarPlane()                       const { return m_far_plane; }
        ProjectionType GetProjectionType()        const { return m_projection_type; }
        //=====================================================================

        //= FOV ==========================================================
        float GetFovHorizontalRad()               const { return m_fov_horizontal_rad; }
        float GetFovVerticalRad()                 const;
        float GetFovHorizontalDeg()               const;
        void SetFovHorizontalDeg(float fov);
        const RHI_Viewport& GetViewport()         const;
        //================================================================

        //= MISC =================================================================================
        bool IsInViewFrustrum(Renderable* renderable) const;
        bool IsInViewFrustrum(const Vector3& center, const Vector3& extents) const;
        const Vector4& GetClearColor()                  const { return m_clear_color; }
        void SetClearColor(const Vector4& color)              { m_clear_color = color; }
        bool GetFpsControlEnabled()                     const { return m_fps_control_enabled; }
        void SetFpsControlEnabled(const bool enabled)         { m_fps_control_enabled = enabled; }
        bool IsFpsControlled()                          const { return m_fps_control_assumed; }
        //========================================================================================

        //= MOUSE ===
        float GetMouseSensitivity()                     const { return m_mouse_sensitivity; }
        float GetMouseSmoothing()                       const { return m_mouse_smoothing; }
        void SetMouseSensitivity(float mouse_sensitivity)     { m_mouse_sensitivity = mouse_sensitivity; }
        void SetMouseSmoothing(float mouse_smoothing)         { m_mouse_smoothing = mouse_smoothing; }
        //===========
        
        //= SPEED ===
        float GetMovementSpeedMin()                     const { return m_movement_speed_min; }
        float GetMovementSpeedMax()                     const { return m_movement_speed_max; }
        float GetMovementAcceleration()                 const { return m_movement_acceleration; }
        float GetMovementDrag()                         const { return m_movement_drag; }
        void SetMovementSpeedMin(float speed_min)             { m_movement_speed_min = speed_min; }
        void SetMovementSpeedMax(float speed_max)             { m_movement_speed_max = speed_max; }
        void SetMovementAcceleration(float acceleration)      { m_movement_acceleration = acceleration; }
        void SetMovementDrag(float drag)                      { m_movement_drag = drag; }
        //===========

        Matrix ComputeViewMatrix() const;
        Matrix ComputeProjection(const bool reverse_z, const float near_plane = 0.0f, const float far_plane = 0.0f);

    private:
        void FpsControl(float delta_time);

        float m_aperture                    = 50.0f;        // Size of the lens diaphragm (mm). Controls depth of field and chromatic aberration.
        float m_shutter_speed               = 1.0f / 60.0f; // Length of time for which the camera shutter is open (sec). Also controls the amount of motion blur.
        float m_iso                         = 500.0f;       // Sensitivity to light.
        float m_fov_horizontal_rad          = DegreesToRadians(90.0f);
        float m_near_plane                  = 0.3f;
        float m_far_plane                   = 1000.0f;
        ProjectionType m_projection_type    = Projection_Perspective;
        Vector4 m_clear_color               = Vector4(0.396f, 0.611f, 0.937f, 1.0f); // A nice cornflower blue 
        Matrix m_view                       = Matrix::Identity;
        Matrix m_projection                 = Matrix::Identity;
        Matrix m_view_projection            = Matrix::Identity;
        Vector3 m_position                  = Vector3::Zero;
        Quaternion m_rotation               = Quaternion::Identity;
        bool m_is_dirty                     = false;
        bool m_fps_control_enabled          = true;
        bool m_fps_control_assumed          = false;
        Vector2 m_mouse_last_position       = Vector2::Zero;
        bool m_fps_control_cursor_hidden    = false;
        Vector3 m_movement_speed            = Vector3::Zero;
        float m_movement_speed_min          = 0.5f;
        float m_movement_speed_max          = 20.0f;
        float m_movement_acceleration       = 1000.0f;
        float m_movement_drag               = 10.0f;
        Vector2 m_mouse_smoothed            = Vector2::Zero;
        Vector2 m_mouse_rotation            = Vector2::Zero;
        float m_mouse_sensitivity           = 1.0f;
        float m_mouse_smoothing             = 0.5f;
        RHI_Viewport m_last_known_viewport;
        Ray m_ray;
        Frustum m_frustrum;

        // Dependencies
        Renderer* m_renderer    = nullptr;
        Input* m_input          = nullptr;
    };
}
