#pragma once

//= INCLUDES ================
#include "Widget.h"
#include "RHI/RHI_Viewport.h"
//===========================

//= FORWARD DECLARATIONS =
namespace Genome
{
    class Renderer;
    class Settings;
    class World;
    class Input;
}
//========================

using namespace Genome;

class Widget_Viewport : public Widget
{
public:
    Widget_Viewport(Editor* editor);

    void TickVisible() override;

private:
    float m_width = 0.0f;
    float m_height = 0.0f;
    Math::Vector2 m_offset = Math::Vector2::Zero;
    float m_window_padding = 4.0f;
    bool m_is_resolution_dirty = true;
    Renderer* m_renderer = nullptr;
    Settings* m_settings = nullptr;
    World* m_world = nullptr;
    Input* m_input = nullptr;
};
