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

//= INCLUDES ====================================
#include "Widget.h"
#include <memory>
#include "../WidgetsDeferred/ButtonColorPicker.h"
//===============================================

namespace Genome
{
    class Entity;
    class Transform;
    class Light;
    class Renderable;
    class RigidBody;
    class SoftBody;
    class Collider;
    class Constraint;
    class Material;
    class Camera;
    class AudioSource;
    class AudioListener;
    class Script;
    class Terrain;
    class Environment;
    class IComponent;
}

class Widget_Properties : public Widget
{
public:
    Widget_Properties(Editor* editor);

    void TickVisible() override;

    static void Inspect(const std::weak_ptr<Genome::Entity>& entity);
    static void Inspect(const std::weak_ptr<Genome::Material>& material);

    // Inspected resources
    static std::weak_ptr<Genome::Entity> m_inspected_entity;
    static std::weak_ptr<Genome::Material> m_inspected_material;

private:
    void ShowTransform(Genome::Transform* transform) const;
    void ShowLight(Genome::Light* light) const;
    void ShowRenderable(Genome::Renderable* renderable) const;
    void ShowRigidBody(Genome::RigidBody* rigid_body) const;
    void ShowSoftBody(Genome::SoftBody* soft_body) const;
    void ShowCollider(Genome::Collider* collider) const;
    void ShowConstraint(Genome::Constraint* constraint) const;
    void ShowMaterial(Genome::Material* material) const;
    void ShowCamera(Genome::Camera* camera) const;
    void ShowEnvironment(Genome::Environment* environment) const;
    void ShowTerrain(Genome::Terrain* terrain) const;
    void ShowAudioSource(Genome::AudioSource* audio_source) const;
    void ShowAudioListener(Genome::AudioListener* audio_listener) const;
    void ShowScript(Genome::Script* script) const;

    void ShowAddComponentButton() const;
    void ComponentContextMenu_Add() const;
    void Drop_AutoAddComponents() const;

    // Color pickers
    std::unique_ptr<ButtonColorPicker> m_colorPicker_material;
    std::unique_ptr<ButtonColorPicker> m_colorPicker_light;
    std::unique_ptr<ButtonColorPicker> m_colorPicker_camera;
};
