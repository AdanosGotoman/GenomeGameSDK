//= INCLUDES ==================
#include "Widget_Viewport.h"
#include "Core/Timer.h"
#include "Core/Settings.h"
#include "Rendering/Model.h"
#include "../ImGui_Extension.h"
//=============================

//= NAMESPACES =========
using namespace std;
using namespace Genome;
using namespace Math;
//======================

Widget_Viewport::Widget_Viewport(Editor* editor) : Widget(editor)
{
    m_title = "Viewport";
    m_size = Vector2(400, 250);
    m_flags |= ImGuiWindowFlags_NoScrollbar;
    m_padding = Vector2(4.0f);
    m_world = m_context->GetSubsystem<World>();
    m_renderer = m_context->GetSubsystem<Renderer>();
    m_settings = m_context->GetSubsystem<Settings>();
    m_input = m_context->GetSubsystem<Input>();
}

void Widget_Viewport::TickVisible()
{
    if (!m_renderer)
        return;

    // Get size
    float width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
    float height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

    // Get offset
    Vector2 offset = Vector2(ImGui::GetWindowPos()) + m_window_padding;

    // Update engine's viewport
    if (m_width != width || m_height != height || m_offset != offset)
    {
        m_renderer->SetViewport(width, height, offset.x, offset.y);

        m_width = width;
        m_height = height;
        m_offset = offset;
    }

    // If this is the first tick and the first time the engine runs (no settings file loaded), we set the resolution to match the viewport size.
    // This is to avoid a scenario were the resolution is much higher than what the user assumes, resulting in less performance.
    if (m_is_resolution_dirty && !m_settings->Loaded())
    {
        m_renderer->SetResolutionRender(static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));
        m_is_resolution_dirty = false;
    }

    // Draw the image after a potential Renderer::SetResolution() call has been made
    ImGuiEx::Image
    (
        m_renderer->GetFrameTexture(),
        ImVec2(static_cast<float>(m_width), static_cast<float>(m_height)),
        ImColor(255, 255, 255, 255),
        ImColor(50, 127, 166, 255)
    );

    // Let the input system now if the mouse is within the viewport
    m_input->SetMouseIsInViewport(ImGui::IsItemHovered());

    // If this widget was released, make the engine pick an entity.
    if (ImGui::IsMouseDown(0) && ImGui::IsItemHovered())
    {
        EditorHelper::Get().PickEntity();
    }

    // Handle model drop
    if (auto payload = ImGuiEx::ReceiveDragPayload(ImGuiEx::DragPayload_Model))
    {
        EditorHelper::Get().LoadModel(get<const char*>(payload->data));
    }
}
