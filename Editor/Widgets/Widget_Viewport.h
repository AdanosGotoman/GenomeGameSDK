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

class Widget_Viewport : public Widget
{
public:
    Widget_Viewport(Editor* editor);

    void TickVisible() override;

private:
    float m_width                   = 0.0f;
    float m_height                  = 0.0f;
    Genome::Math::Vector2 m_offset = Genome::Math::Vector2::Zero;
    float m_window_padding          = 4.0f;
    bool m_is_resolution_dirty      = true;
    Genome::Renderer* m_renderer   = nullptr;
    Genome::Settings* m_settings   = nullptr;
    Genome::World* m_world         = nullptr;
    Genome::Input* m_input         = nullptr;
};
