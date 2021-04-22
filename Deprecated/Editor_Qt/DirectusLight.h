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

#pragma once

//==================================
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QDoubleValidator>
#include <memory>
#include "DirectusIComponent.h"
//==================================

class DirectusComboLabelText;
class DirectusInspector;
class DirectusColorPicker;
class DirectusComboSliderText;
namespace Directus
{
    class GameObject;
    class Light;
}

class DirectusLight : public DirectusIComponent
{
    Q_OBJECT
public:
    DirectusLight();

    virtual void Initialize(DirectusInspector* inspector, QWidget* mainWindow);
    virtual void Reflect(std::weak_ptr<Directus::GameObject> gameobject);

private:
    //= LIGHT TYPE =======================
    QLabel* m_lightTypeLabel;
    QComboBox* m_lightType;
    //====================================

    //= ANGLE ============================
    DirectusComboLabelText* m_angle;
    //====================================

    //= RANGE ============================
    DirectusComboLabelText* m_range;
    //====================================

    //= COLOR ============================
    QLabel* m_colorLabel;
    DirectusColorPicker* m_color;
    //====================================

    //= INTENSTITY =======================
    QLabel* m_intensityLabel;
    DirectusComboSliderText* m_intensity;
    //====================================

    //= SHADOW TYPE ======================
    QLabel* m_shadowTypeLabel;
    QComboBox* m_shadowType;
    //====================================

    //= MISC =============================
    QValidator* m_validator;
    Directus::Light* m_inspectedLight;
    //====================================

    void ReflectLightType();
    void ReflectAngle();
    void ReflectRange();
    void ReflectColor();
    void ReflectIntensity();
    void ReflectShadowType();

public slots:
    void MapLightType();
    void MapAngle();
    void MapRange();
    void MapColor();
    void MapIntensity();
    void MapShadowType();
    virtual void Remove();
};
