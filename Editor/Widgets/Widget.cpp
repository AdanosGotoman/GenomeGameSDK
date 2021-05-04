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

//= INCLUDES ==============================
#include "Widget.h"
#include "Core/Context.h"
#include "../Editor.h"
#include "../ImGui/Source/imgui_internal.h"
#include "Profiling/Profiler.h"
//=========================================

Widget::Widget(Editor* editor)
{
    m_editor    = editor;
    m_context   = editor->GetContext();
    m_profiler  = m_context->GetSubsystem<Genome::Profiler>();
    m_window    = nullptr;
}

void Widget::TickAlways() { }

void Widget::TickVisible() { }

void Widget::OnShow() { }

void Widget::OnHide() { }

void Widget::OnPushStyleVar() { }

void Widget::Tick()
{
    TickAlways();

    if (!m_is_window)
        return;

    // ImGui::Begin()
    bool begun = false;
    {
        if (!m_is_visible)
            return;

        TIME_BLOCK_START_NAMED(m_profiler, m_title.c_str());

        // Position
        if (m_position.x != -1.0f && m_position.y != -1.0f)
        {
            ImGui::SetNextWindowPos(m_position);
        }

        // Padding
        if (m_padding.x != -1.0f && m_padding.y != -1.0f)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_padding);
            m_var_pushes++;
        }

        // Alpha
        if (m_alpha != -1.0f)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_alpha);
            m_var_pushes++;
        }

        // Size
        if (m_size.x != -1.0f && m_size.y != -1.0f)
        {
            ImGui::SetNextWindowSize(m_size, ImGuiCond_FirstUseEver);
        }

        // Max size
        if ((m_size.x != -1.0f && m_size.y != -1.0f) || (m_size_max.x != FLT_MAX && m_size_max.y != FLT_MAX))
        {
            ImGui::SetNextWindowSizeConstraints(m_size, m_size_max);
        }

        // Callback
        OnPushStyleVar();

        // Begin
        if (ImGui::Begin(m_title.c_str(), &m_is_visible, m_flags))
        {
            m_window = ImGui::GetCurrentWindow();
            m_height = ImGui::GetWindowHeight();
            begun = true;
        }
        else if (m_window && m_window->Hidden)
        {
            // Enters here if the window is hidden as part of an unselected tab.
            // ImGui::Begin() makes the window but returns false, then ImGui still expects ImGui::End() to be called.
            // So we make sure that when Widget::End() is called, ImGui::End() get's called as well.
            // Note: ImGui's docking is in beta, so maybe it's at fault here ?
            begun = true;
        }

        // Callbacks
        if (m_window && m_window->Appearing)
        {
            OnShow();
        }
        else if (!m_is_visible)
        {
            OnHide();
        }
    }

    if (begun)
    {
        TickVisible();

        // ImGui::End()
        {
            // End
            ImGui::End();

            // Pop style variables
            ImGui::PopStyleVar(m_var_pushes);
            m_var_pushes = 0;

            // End profiling
            TIME_BLOCK_END(m_profiler);
        }
    }
}
