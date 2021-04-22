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

//= INCLUDES =============================
#include "Spartan.h"
#include "Renderer.h"
#include "../World/Components/Camera.h"
#include "../World/Components/Transform.h"
//========================================

//= NAMESPACES ===============
using namespace std;
using namespace Genome::Math;
//============================

namespace Genome
{
    void Renderer::TickPrimitives(const float delta_time)
    {
        // Remove lines which have expired

        uint32_t end = static_cast<uint32_t>(m_lines_depth_disabled_duration.size());
        for (uint32_t i = 0; i < end; i++)
        {
            m_lines_depth_disabled_duration[i] -= delta_time;
        
            if (m_lines_depth_disabled_duration[i] <= 0.0f)
            {
                m_lines_depth_disabled_duration.erase(m_lines_depth_disabled_duration.begin() + i);
                m_lines_depth_disabled.erase(m_lines_depth_disabled.begin() + i); 
                i--;
                end--;
            }
        }
        
        end = static_cast<uint32_t>(m_lines_depth_enabled_duration.size());
        for (uint32_t i = 0; i < end; i++)
        {
            m_lines_depth_enabled_duration[i] -= delta_time;
        
            if (m_lines_depth_enabled_duration[i] <= 0.0f)
            {
                m_lines_depth_enabled_duration.erase(m_lines_depth_enabled_duration.begin() + i);
                m_lines_depth_enabled.erase(m_lines_depth_enabled.begin() + i);
                i--;
                end--;
            }
        }
    }

    void Renderer::DrawLine(const Vector3& from, const Vector3& to, const Vector4& color_from, const Vector4& color_to, const float duration /*= 0.0f*/, const bool depth /*= true*/)
    {
        if (depth)
        {
            m_lines_depth_enabled.emplace_back(from, color_from);
            m_lines_depth_enabled_duration.emplace_back(duration);

            m_lines_depth_enabled.emplace_back(to, color_to);
            m_lines_depth_enabled_duration.emplace_back(duration);
        }
        else
        {
            m_lines_depth_disabled.emplace_back(from, color_from);
            m_lines_depth_disabled_duration.emplace_back(duration);

            m_lines_depth_disabled.emplace_back(to, color_to);
            m_lines_depth_disabled_duration.emplace_back(duration);
        }
    }

    void Renderer::DrawTriangle(const Math::Vector3& v0, const Math::Vector3& v1, const Math::Vector3& v2, const Math::Vector4& color /*= DEBUG_COLOR*/, const float duration /*= 0.0f*/, bool depth /*= true*/)
    {
        DrawLine(v0, v1, color, color, duration, depth);
        DrawLine(v1, v2, color, color, duration, depth);
        DrawLine(v2, v0, color, color, duration, depth);
    }

    void Renderer::DrawRectangle(const Math::Rectangle& rectangle, const Math::Vector4& color /*= DebugColor*/, const float duration /*= 0.0f*/, bool depth /*= true*/)
    {
        const float cam_z = m_camera->GetTransform()->GetPosition().z + m_camera->GetNearPlane() + 5.0f;

        DrawLine(Vector3(rectangle.left,    rectangle.top,      cam_z), Vector3(rectangle.right,    rectangle.top,      cam_z), color, color, duration, depth);
        DrawLine(Vector3(rectangle.right,   rectangle.top,      cam_z), Vector3(rectangle.right,    rectangle.bottom,   cam_z), color, color, duration, depth);
        DrawLine(Vector3(rectangle.right,   rectangle.bottom,   cam_z), Vector3(rectangle.left,     rectangle.bottom,   cam_z), color, color, duration, depth);
        DrawLine(Vector3(rectangle.left,    rectangle.bottom,   cam_z), Vector3(rectangle.left,     rectangle.top,      cam_z), color, color, duration, depth);
    }

    void Renderer::DrawBox(const BoundingBox& box, const Vector4& color, const float duration /*= 0.0f*/, const bool depth /*= true*/)
    {
        const Vector3& min = box.GetMin();
        const Vector3& max = box.GetMax();
    
        DrawLine(Vector3(min.x, min.y, min.z), Vector3(max.x, min.y, min.z), color, color, duration, depth);
        DrawLine(Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, min.z), color, color, duration, depth);
        DrawLine(Vector3(max.x, max.y, min.z), Vector3(min.x, max.y, min.z), color, color, duration, depth);
        DrawLine(Vector3(min.x, max.y, min.z), Vector3(min.x, min.y, min.z), color, color, duration, depth);
        DrawLine(Vector3(min.x, min.y, min.z), Vector3(min.x, min.y, max.z), color, color, duration, depth);
        DrawLine(Vector3(max.x, min.y, min.z), Vector3(max.x, min.y, max.z), color, color, duration, depth);
        DrawLine(Vector3(max.x, max.y, min.z), Vector3(max.x, max.y, max.z), color, color, duration, depth);
        DrawLine(Vector3(min.x, max.y, min.z), Vector3(min.x, max.y, max.z), color, color, duration, depth);
        DrawLine(Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z), color, color, duration, depth);
        DrawLine(Vector3(max.x, min.y, max.z), Vector3(max.x, max.y, max.z), color, color, duration, depth);
        DrawLine(Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z), color, color, duration, depth);
        DrawLine(Vector3(min.x, max.y, max.z), Vector3(min.x, min.y, max.z), color, color, duration, depth);
    }

    void Renderer::DrawCircle(const Math::Vector3& center, const Math::Vector3& axis, const float radius, const uint32_t segmentCount, const Math::Vector4& color /*= DEBUG_COLOR*/, const float duration /*= 0.0f*/, const bool depth /*= true*/)
    {
        if (radius <= 0.0f || segmentCount <= 0)
            return;

        vector<Vector3> points;
        points.reserve(segmentCount + 1);
        points.resize(segmentCount + 1);

        // Compute points on circle
        float angle_step = Math::Helper::PI_2 / (float)segmentCount;
        for (uint32_t i = 0; i <= segmentCount; i++)
        {
            float angle = (float)i * angle_step;
            if (axis.x != 0.0f)
            {
                points[i] = Vector3(center.x, cos(angle) * radius + center.y, sin(angle) * radius + center.z);
            }
            else if (axis.y != 0.0f)
            {
                points[i] = Vector3(cos(angle) * radius + center.x, center.y, sin(angle) * radius + center.z);
            }
            else
            {
                points[i] = Vector3(cos(angle) * radius + center.x, sin(angle) * radius + center.y, center.z);
            }
        }

        // Draw
        for (uint32_t i = 0; i <= segmentCount - 1; i++)
        {
            DrawLine(points[i], points[i + 1], color, color, duration, depth);
        }
    }
}
