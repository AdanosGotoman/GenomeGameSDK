/*
Copyright(c) 2016 Panos Karabelas

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

//= INCLUDES ===================
#include "DirectusCamera.h"
#include "DirectusInspector.h"
#include "DirectusAdjustLabel.h"
#include "Logging/Log.h"
//==============================

//= NAMESPACES ================
using namespace std;
using namespace Directus;
using namespace Directus::Math;
//=============================

DirectusCamera::DirectusCamera()
{

}

void DirectusCamera::Initialize(DirectusInspector* inspector, QWidget* mainWindow)
{

    m_inspector = inspector;

    m_gridLayout = new QGridLayout();
    m_gridLayout->setMargin(4);

    //= TITLE =================================================
    m_title = new QLabel("Camera");
    m_title->setStyleSheet(
                "background-image: url(:/Images/camera.png);"
                "background-repeat: no-repeat;"
                "background-position: left;"
                "padding-left: 20px;"
                );

    m_optionsButton = new DirectusDropDownButton();
    m_optionsButton->Initialize(mainWindow);
    //=========================================================

    //= BACKGROUND ============================================
    m_backgroundLabel = new QLabel("Background");
    m_background = new DirectusColorPicker();
    m_background->Initialize(mainWindow);
    //=========================================================

    //= PROJECTION ============================================
    m_projectionLabel = new QLabel("Projection");
    m_projectionComboBox = new QComboBox();
    m_projectionComboBox->addItem("Perspective");
    m_projectionComboBox->addItem("Orthographic");
    //=========================================================

    //= FOV ===================================================
    m_fovLabel = new QLabel("Field of view");
    m_fov = new DirectusComboSliderText();
    m_fov->Initialize(1, 179);
    //=========================================================

    //= CLIPPING PLANES ==========================================================
    m_clippingPlanesLabel = new QLabel("Clipping planes");

    m_nearPlane = new DirectusComboLabelText();
    m_nearPlane->Initialize("Near");

    m_farPlane = new DirectusComboLabelText();
    m_farPlane->Initialize("Far");
    //=============================================================================

    //= LINE ======================================
    m_line = new QWidget();
    m_line->setFixedHeight(1);
    m_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_line->setStyleSheet(QString("background-color: #585858;"));
    //=============================================

    // addWidget(widget, row, column, rowspan, colspan)
    //= GRID ======================================================================
    // Row 0
    m_gridLayout->addWidget(m_title, 0, 0, 1, 1);
    m_gridLayout->addWidget(m_optionsButton, 0, 3, 1, 1, Qt::AlignRight);

    // Row 1
    m_gridLayout->addWidget(m_backgroundLabel,          1, 0, 1, 1);
    m_gridLayout->addWidget(m_background->GetWidget(),  1, 1, 1, 3);

    // Row 2
    m_gridLayout->addWidget(m_projectionLabel,      2, 0, 1, 1);
    m_gridLayout->addWidget(m_projectionComboBox,   2, 1, 1, 3);

    // Row 3
    m_gridLayout->addWidget(m_fovLabel,             3, 0, 1, 1);
    m_gridLayout->addWidget(m_fov->GetSlider(),     3, 1, 1, 2);
    m_gridLayout->addWidget(m_fov->GetLineEdit(),   3, 3, 1, 1);

    // Row 4 and 5
    m_gridLayout->addWidget(m_clippingPlanesLabel,          4, 0, 1, 1);
    m_gridLayout->addWidget(m_nearPlane->GetLabelWidget(),  4, 1, 1, 1);
    m_gridLayout->addWidget(m_nearPlane->GetTextWidget(),   4, 2, 1, 2);
    m_gridLayout->addWidget(m_farPlane->GetLabelWidget(),   5, 1, 1, 1);
    m_gridLayout->addWidget(m_farPlane->GetTextWidget(),    5, 2, 1, 2);

    // Row 6 - LINE
    m_gridLayout->addWidget(m_line, 6, 0, 1, 4);
    //=============================================================================

    //= SET GRID SPACING ===================================
    m_gridLayout->setHorizontalSpacing(m_horizontalSpacing);
    m_gridLayout->setVerticalSpacing(m_verticalSpacing);
    //======================================================

    // textChanged(QString) -> emits signal when changed through code
    // textEdit(QString) -> doesn't emit signal when changed through code
    connect(m_optionsButton,        SIGNAL(Remove()),                   this, SLOT(Remove()));
    connect(m_background,           SIGNAL(ColorPickingCompleted()),    this, SLOT(MapBackground()));
    connect(m_projectionComboBox,   SIGNAL(activated(int)),             this, SLOT(MapProjection()));
    connect(m_fov,                  SIGNAL(ValueChanged()),             this, SLOT(MapFOV()));
    connect(m_nearPlane,            SIGNAL(ValueChanged()),             this, SLOT(MapNearPlane()));
    connect(m_farPlane,             SIGNAL(ValueChanged()),             this, SLOT(MapFarPlane()));

    this->setLayout(m_gridLayout);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    this->hide();
}

void DirectusCamera::Reflect(std::weak_ptr<Directus::GameObject> gameobject)
{
    m_inspectedCamera = nullptr;

    // Catch the evil case
    if (gameobject.expired())
    {
        this->hide();
        return;
    }

    // Catch the seed of the evil
    m_inspectedCamera = gameobject.lock()->GetComponent<Camera>().lock().get();
    if (!m_inspectedCamera)
    {
        this->hide();
        return;
    }

    // Do the actual reflection
    ReflectBackground(m_inspectedCamera->GetClearColor());
    ReflectProjection(m_inspectedCamera->GetProjection());
    ReflectFOV(m_inspectedCamera->GetFOV_Horizontal_Deg());
    ReflectNearPlane(m_inspectedCamera->GetNearPlane());
    ReflectFarPlane(m_inspectedCamera->GetFarPlane());

    // Make this widget visible
    this->show();

}

void DirectusCamera::ReflectBackground(Directus::Math::Vector4 color)
{
    m_background->SetColor(color);
}

void DirectusCamera::ReflectProjection(Projection projection)
{
    m_projectionComboBox->setCurrentIndex((int)projection);
}

void DirectusCamera::ReflectNearPlane(float nearPlane)
{
    m_nearPlane->SetFromFloat(nearPlane);
}

void DirectusCamera::ReflectFarPlane(float farPlane)
{
    m_farPlane->SetFromFloat(farPlane);
}

void DirectusCamera::ReflectFOV(float fov)
{
    m_fov->SetValue(fov);
}

void DirectusCamera::MapBackground()
{
    if(!m_inspectedCamera)
        return;

    Vector4 clearColor = m_background->GetColor();
    m_inspectedCamera->SetClearColor(clearColor);
}

void DirectusCamera::MapProjection()
{
    if(!m_inspectedCamera)
        return;

    Projection projection = (Projection)(m_projectionComboBox->currentIndex());
    m_inspectedCamera->SetProjection(projection);
}

void DirectusCamera::MapFOV()
{
    if(!m_inspectedCamera)
        return;

    float fov = m_fov->GetValue();
    m_inspectedCamera->SetFOV_Horizontal_Deg(fov);
}

void DirectusCamera::MapNearPlane()
{
    if(!m_inspectedCamera)
        return;

    float nearPlane = m_nearPlane->GetAsFloat();
    m_inspectedCamera->SetNearPlane(nearPlane);
}

void DirectusCamera::MapFarPlane()
{
    if(!m_inspectedCamera)
        return;

    float farPlane = m_farPlane->GetAsFloat();
    m_inspectedCamera->SetFarPlane(farPlane);
}

void DirectusCamera::Remove()
{
    if (!m_inspectedCamera)
        return;

    auto gameObject = m_inspectedCamera->GetGameObjectRef();
    if (!gameObject.expired())
    {
        gameObject.lock()->RemoveComponent<Camera>();
    }

    m_inspector->Inspect(gameObject);
}
