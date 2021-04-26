#pragma once

//= INCLUDES ======
#include "Widget.h"
//=================

namespace Genome { class Renderer; }

class Widget_RenderOptions : public Widget
{
public:
    Widget_RenderOptions(Editor* editor);

    void TickVisible() override;

private:
    Genome::Renderer* m_renderer;
};
