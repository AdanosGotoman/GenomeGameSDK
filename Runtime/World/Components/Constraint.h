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

//= INCLUDES =====================
#include "IComponent.h"
#include "../../Math/Vector3.h"
#include "../../Math/Vector2.h"
#include "../../Math/Quaternion.h"
//================================

using namespace Genome::Math;

class btTypedConstraint;

namespace Genome
{
    class RigidBody;
    class Entity;
    class Physics;

    enum ConstraintType
    {
        ConstraintType_Point,
        ConstraintType_Hinge,
        ConstraintType_Slider,
        ConstraintType_ConeTwist
    };

    class GENOME_CLASS Constraint : public IComponent
    {
    public:
        Constraint(Context* context, Entity* entity, uint32_t id = 0);
        ~Constraint();

        //= COMPONENT ================================
        void OnInitialize() override;
        void OnStart() override;
        void OnStop() override;
        void OnRemove() override;
        void OnTick(float delta_time) override;
        void Serialize(FileStream* stream) override;
        void Deserialize(FileStream* stream) override;
        //============================================

        ConstraintType GetConstraintType()        const { return m_constraintType; }
        void SetConstraintType(ConstraintType type);

        const Vector2& GetHighLimit()             const { return m_highLimit; }
        // Set high limit. Interpretation is constraint type specific.
        void SetHighLimit(const Vector2& limit);

        const Vector2& GetLowLimit()              const { return m_lowLimit; }
        // Set low limit. Interpretation is constraint type specific.
        void SetLowLimit(const Vector2& limit);

        const Vector3& GetPosition()              const { return m_position; }
        // Set constraint position relative to own body.
        void SetPosition(const Vector3& position);

        const Quaternion& GetRotation()           const { return m_rotation; }
        // Set constraint rotation relative to own body.
        void SetRotation(const Quaternion& rotation);

        const Vector3& GetPositionOther()         const { return m_positionOther; }
        // Set constraint position relative to other body.
        void SetPositionOther(const Vector3& position);

        const Quaternion& GetRotationOther()      const { return m_rotationOther; }
        // Set constraint rotation relative to other body.
        void SetRotationOther(const Quaternion& rotation);
        
        std::weak_ptr<Entity> GetBodyOther()      const { return m_bodyOther; }
        void SetBodyOther(const std::weak_ptr<Entity>& body_other);

        void ReleaseConstraint();
        void ApplyFrames() const;

    private:
        void Construct();
        void ApplyLimits() const;
        
        btTypedConstraint* m_constraint;

        ConstraintType m_constraintType;
        Vector3 m_position;
        Quaternion m_rotation;
        Vector2 m_highLimit;
        Vector2 m_lowLimit;

        std::weak_ptr<Entity> m_bodyOther;
        Vector3 m_positionOther;
        Quaternion m_rotationOther;
    
        float m_errorReduction;
        float m_constraintForceMixing;
        bool m_enabledEffective;
        bool m_collisionWithLinkedBody;
        bool m_deferredConstruction;

        Physics* m_physics;
    };
}
