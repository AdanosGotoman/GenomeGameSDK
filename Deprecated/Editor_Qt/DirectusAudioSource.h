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

//=====================================
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QCheckBox>
#include "DirectusViewport.h"
#include "DirectusComboSliderText.h"
#include "DirectusDropDownButton.h"
#include "DirectusIComponent.h"
#include "DirectusAudioClipDropTarget.h"
#include "Components/AudioSource.h"
//======================================

class DirectusInspector;

class DirectusAudioSource : public DirectusIComponent
{
    Q_OBJECT
public:
    DirectusAudioSource();

    virtual void Initialize(DirectusInspector* inspector, QWidget* mainWindow);
    virtual void Reflect(std::weak_ptr<Directus::GameObject> gameobject);

private:
    //= AUDIO CLIP ==========================
    QLabel* m_audioClipLabel;
    DirectusAudioClipDropTarget* m_audioClip;
    //=======================================

    //= MUTE ===========================
    QLabel* m_muteLabel;
    QCheckBox* m_muteCheckBox;
    //==================================

    //= PLAY ON AWAKE ==================
    QLabel* m_playOnAwakeLabel;
    QCheckBox* m_playOnAwakeCheckBox;
    //==================================

    //= LOOP ===========================
    QLabel* m_loopLabel;
    QCheckBox* m_loopCheckBox;
    //==================================

    //= PRIORITY =======================
    QLabel* m_priorityLabel;
    DirectusComboSliderText* m_priority;
    //==================================

    //= VOLUME =======================
    QLabel* m_volumeLabel;
    DirectusComboSliderText* m_volume;
    //================================

    //= PITCH ========================
    QLabel* m_pitchLabel;
    DirectusComboSliderText* m_pitch;
    //================================

    //= PITCH ========================
    QLabel* m_panLabel;
    DirectusComboSliderText* m_pan;
    //================================

    //= MISC ===============
    QValidator* m_validator;
    //======================

    //= MISC =====================================
    Directus::AudioSource* m_inspectedAudioSource;
    //============================================

    void ReflectName();
    void ReflectMute();
    void ReflectPlayOnAwake();
    void ReflectLoop();
    void ReflectPriority();
    void ReflectVolume();
    void ReflectPitch();
    void ReflectPan();

public slots:
    void MapMute();
    void MapPlayOnAwake();
    void MapLoop();
    void MapPriority();
    void MapVolume();
    void MapPitch();
    void MapPan();

    virtual void Remove();
};
